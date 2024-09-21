#include "stdafx.h"
#include "Source.h"
#include <atlbase.h>
#include <boost/geometry/strategies/strategy_transform.hpp>

using namespace boost::geometry;
using namespace boost::geometry::strategy::transform;

namespace
{
	Point3 normalize(const Point3& n)
	{
		Point3 norm(n);
		double x = n.get<0>();
		double y = n.get<1>();
		double z = n.get<2>();
		auto len = std::sqrt(x*x + y*y + z*z);
		multiply_value(norm, 1 / len);
		return norm;
	}
}

Source::Source(Point3 origin, Point3 normal) :
	origin_(origin), normal_(normalize(normal))
{ 
}

Point3 Source::getOrigin() const
{
	return origin_;
}

Point3 Source::getNormal() const
{
	return normal_;
}

// local -> global conversion
Point3 Source::getGlobal(const Point2& uv) const
{
	Point3 lp(uv.get<0>(), uv.get<1>(), zLevel());

	return lp;
}

// local -> global conversion
Point2 Source::getLocal(const Point3& p) const
{
	Point2 uv(p.get<0>(), p.get<1>());

	return uv;
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
	auto p = getGlobal(node);
	double x = p.get<0>();
	double y = p.get<1>();
	if (y == 0)
		y = 0.01;
	node.u = field_t(x * y, x / y * PI / 5);
	complex_vector grad{ y, x, PI / (5 * y), - PI*x / (y*y)};
	node.grad = grad;
	Point3 Bre{ PI / (5 * y), -PI * x / (y * y), PI * (x + y) };
	Point3 Bim{ PI / (-5 * y), PI * x / (y * y), PI * (x - y) };
	add_point(node.Bre, Bre);
	add_point(node.Bim, Bim);

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

Point3 OrthoPlaneSource::getGlobal(const Point2& uv) const
{
	Point3 lp(0 , 0, 0);

	// сдвиг
	lp.set<0>(uv.get<0>() - getOrigin().get<0>());
	lp.set<1>(uv.get<1>() - getOrigin().get<1>());
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

// local -> global conversion
Point2 OrthoPlaneSource::getLocal(const Point3& p) const
{
	auto x = p.get<0>();
	auto y = p.get<1>();
	auto z = p.get<2>();

	auto x0 = getOrigin().get<0>();
	auto y0 = getOrigin().get<1>();
	auto nx = getNormal().get<0>();
	auto ny = getNormal().get<1>();

	auto u = (x - x0) * nx + (y - y0) * ny;
	auto v = -(x - x0) * ny + (y - y0) * nx;

	// check
	{
		auto bx = u * nx - v * ny + x0;
		auto by = u * ny + v * nx + y0;
		int i = 5;
	}
	return Point2(u, v);
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
	
	// возвращает длину вектора из начала координат в точку p
	double lenv(const Point3& p)
	{
		return distance(p, Point3(0, 0, 0));
	}

	// вычисляет расстояние от точки p до прямой p1-p2
	double pointToLine(Point3 p, Point3 p1, Point3 p2)
	{
		// R = (p2 - p)x(p2 -p1) / |s|
		auto s = p2;
		subtract_point(s, p1);	// p2 - p1
		auto pm = p2;
		subtract_point(pm, p);	// p2 - p
		return lenv(cross_product(pm, s)) / lenv(s);
	}

	// Вычисляет косинус угла между векторами v1 и v2
	double cosvv(const Point3& v1, const Point3& v2)
	{
		auto m = lenv(v1) * lenv(v2);
		return m == 0 ? 0 : dot_product(v1, v2) / m;
	}
}

void OrthoPlaneSource::fillNode(Node& node) const
{
	auto p = Point3(node.get<0>(), node.get<1>(), zLevel());
	auto uv = getLocal(p);
	double z = uv.get<0>();
	double x = -uv.get<1>();
	double y = zLevel();


	if (isInsideBox(Point2(x, z)))
	{
		field_t potential(0, 0);
		complex_vector grad;
		IResultPtr res = pbm_->Result;

		if (pbm_->ProblemType == qfTimeHarmonicElectric)
		{
			FieldPointECPtr field = res->GetLocalValues(QF()->PointXY(x, y));
			potential = field_t(field->U->Re, field->U->Im);
			grad = toComplexVector(field->E);
		}
		else if (pbm_->ProblemType == qfTimeHarmonicMagnetics)
		{
			FieldPointHEPtr field = res->GetLocalValues(QF()->PointXY(x, y));
			potential = field_t(field->MagnPotenial->Re, field->MagnPotenial->Im);
			grad = toComplexVector(field->FluxDensity);
		}
		else
		{
			assert(!"wrong problem type");
		}

		node.u += potential;
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

Point3 Wire::getGlobal(const Point2& p) const
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
	//										   
	// B = mu0*I/(4*pi*R)(cos(Alpha1) -cos(Alpha2)
	//										   
	//	R - минимальное расстояние от провода p1-p2 до точки p, в которой вычисляется поле,
	//  p1-p2 - начальная и конечная точки отрезка провода,
	//	Alpha1 = угол между проводом и отрезком p1-p,
	//  Alpha2 = 180-(угол между проводом и отрезком p2-p).
	
	Point3 p = getGlobal(node);	// мировые координаты точки, в которой вычисляется поле
	double R = pointToLine(p, end_, start_);
	auto p1_p2 = end_;
	subtract_point(p1_p2, start_);	// вектор из p1 в p2 (проводник с током).
	auto p1_p = p;
	subtract_point(p1_p, start_);	// вектор  из p1 в p
	auto p2_p = p;
	subtract_point(p2_p, end_);	// вектор  из p2 в p
	double absB = mu0 / (4 * PI * R) * (cosvv(p1_p2, p1_p) - cosvv(p1_p2, p2_p)); // заготовка для В - ее надо умножить на ток

	// вычислить направление вектора B: перпендикулярно плоскости, 
	//	проходящей через провод p1-p2 и пробную точку p по правилу правой руки: p1_p2 x p2_p
	auto Bdir = cross_product(p1_p2, p2_p);
	divide_value(Bdir, lenv(Bdir));

	// B(re, im) = dirB * absB * I(im, re) 
	auto Bre = Bdir;
	multiply_value(Bre, absB * load_.real());
	auto Bim = Bdir;
	multiply_value(Bim, absB * load_.imag());
	add_point(node.Bre, Bre);
	add_point(node.Bim, Bim);
}
