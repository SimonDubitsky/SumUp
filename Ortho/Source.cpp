#include "stdafx.h"
#include "Source.h"
#include <atlbase.h>

Point3 Source::getOrigin() const
{
	return origin_;
}

Point3 Source::getNormal() const
{
	return normal_;
}

Point3 Source::globalCoord(const Point2& p) const
{
	Point3 lp(p.get<0>(), p.get<1>(), zLevel());

	return lp;
}

void Source::prepare()
{ }

void Source::release()
{ }

std::string Source::name() const
{
	return "noname";
}

void FakeSource::fillNode(Node& node) const
{
	// just emulator
	auto p = globalCoord(node);
	double x = p.get<0>();
	double y = p.get<1>();
	if (y == 0)
		y = 0.01;
	node.u = field_t(x * y, x / y * PI / 5);
	complex_vector grad{ y, x, PI / (5 * y), - PI*x / (y*y)};
	node.grad = grad;
}
//////////////////////////////////////////////////////////////////////////


OrthoPlaneSource::OrthoPlaneSource(const std::string& problemName, 
	const Point3& origin, const Point3& normal, 
	double minX, double maxX, double minY, double maxY) :
	Source(origin, normal),
	problemName_(problemName),
	box_(minX, maxX, minY, maxY)
{ }

OrthoPlaneSource::OrthoPlaneSource(const std::string& problemName,
	const Point3& origin, const Point3& normal, const Box2& box) :
	Source(origin, normal),
	problemName_(problemName),
	box_(box)
{ }

Point3 OrthoPlaneSource::globalCoord(const Point2& p) const
{
	Point3 lp(0 , 0, 0);

	// сдвиг
	lp.set<0>(p.get<0>() - getOrigin().get<0>());
	lp.set<1>(p.get<1>() - getOrigin().get<1>());
	lp.set<2>(zLevel()); //	высоты считаем совпадающими (dz=0)

	// безусловный поворот на +90 град вокруг оси Х (потому что сечения нормальны к базовой плоскости)
	lp = Point3(lp.get<0>(), lp.get<2>(), -lp.get<1>());

	// поворот относительно оси Y
	Point2 n0(0, -1);
	Point2 n(getNormal().get<0>(), getNormal().get<1>());
	double phi = angle(n0, n);
	double si = sin(phi);
	double co = cos(phi);
	lp = Point3(lp.get<0>() * co - lp.get<2>() * si, lp.get<1>(), lp.get<0>() * si + lp.get<2>() * co);
	
	return lp;
}

bool OrthoPlaneSource::isInsideBox(const Point2& p) const
{
	return box_.min_corner().get<0>() <= p.get<0>() && p.get<0>() <= box_.max_corner().get<0>()
		&& box_.min_corner().get<1>() <= p.get<1>() && p.get<1>() <= box_.max_corner().get<1>();
}

void OrthoPlaneSource::prepare()
{ 
	assert(!problemName_.empty());
	pbm_ = QF()->Problems->Open(problemName_.c_str());

	if (!pbm_->Solved)
		pbm_->SolveProblem(FALSE);
	pbm_->AnalyzeResults();
}

void OrthoPlaneSource::release()
{
	assert(pbm_);
	pbm_->Close(FALSE, "");
	pbm_.Release();
}

std::string OrthoPlaneSource::name() const
{
	return problemName();
}

namespace
{
	complex_vector toComplexVector(const ComplexVectorPtr& cv)
	{
		double reX = cv->Re->X;
		double reY = cv->Re->Y;
		double imX = cv->Im->X;
		double imY = cv->Im->Y;
		return complex_vector(cv->Re->X, cv->Re->Y, cv->Im->X, cv->Im->Y);
	}

	// вычисляет расстояние от точки p до прямой p1-p2
	double pointToLine(Point3 p, Point3 p1, Point3 p2)
	{
		// R = (p2 - p)x(p2 -p1) / |s|
		auto s = p2;
		subtract_point(s, p1);	// p2 - p1
		auto pm = p2;
		subtract_point(pm, p);	// p2 - p
		return length(cross_product(pm, s)) / length(s);
	}
}

void OrthoPlaneSource::fillNode(Node& node) const
{
	auto lp = globalCoord(node);
	double x = lp.get<0>();
	double y = lp.get<1>();
	double z = lp.get<2>();

	if (isInsideBox(Point2(x, z)))
	{
		field_t u(0, 0);
		complex_vector grad;
		IResultPtr res = pbm_->Result;

		if (pbm_->ProblemType == qfTimeHarmonicElectric)
		{
			FieldPointECPtr field = res->GetLocalValues(QF()->PointXY(x, y));
			u = field_t(field->U->Re, field->U->Im);
			grad = toComplexVector(field->E);
		}
		else if (pbm_->ProblemType == qfTimeHarmonicMagnetics)
		{
			FieldPointHEPtr field = res->GetLocalValues(QF()->PointXY(x, y));
			u = field_t(field->MagnPotenial->Re, field->MagnPotenial->Im);
			grad = toComplexVector(field->FluxDensity);
		}
		else
		{
			assert(!"wrong problem type");
		}

		node.u += u;
		node.grad += grad;
	}
}

// источник в виде прямого тонкого провода
Wire::Wire(QfProblemTypes problemType, field_t load, const Point3& p1, Point3& p2, const std::string& name) :
	problemType_(problemType), load_(load), name_(name), 
	start_(p1), end_(p2)
{
}

Wire::Wire(QfProblemTypes problemType, field_t load,
	double x1, double y1, double z1,
	double x2, double y2, double z2, const std::string& name) :

	problemType_(problemType), load_(load), name_(name),
	start_(Point3(x1, y1, z1)), end_(Point3(x2, y2, z2))
{

}

Point3 Wire::globalCoord(const Point2& p) const
{
	Point3 lp(p.get<0>(), p.get<1>(), zLevel());

	return lp;
}

void Wire::prepare()
{

}

// вычисляем поле создаваемое прямым отрезком с током (или потенциалом) в заданной точке.
void Wire::fillNode(Node& node) const
{
	//										   | l2
	// B = mu0*I*L/(4*pi*R) / sqrt(R^2 +L^2) |
	//										   | l1
	//	R - минимальное расстояние от провода то точки, в которой вычисляется поле
	//	l - переменная интегрирования (координата вдоль провода)
	
	Point3 p = globalCoord(node);	// мировые координаты точки, в которой вычисляется поле
	double R = pointToLine(p, end_, start_);
	double L = distance(end_, start_);
	auto B = mu0 * L * load_ / (4 * std::numbers::pi * R);
	B *= 1 / std::sqrt(R * R) - 1 / std::sqrt(R * R + L * L);
	node.u += 0;

	// вычислить направление вектора B
	complex_vector dirB(1, 0, 1, 0);		// условно
	//dirB.real *= B.real;
	//dirB.imag *= B.imag;
	//node.grad = node.grad + dirB;
}
