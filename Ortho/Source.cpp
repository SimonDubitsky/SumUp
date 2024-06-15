#include "stdafx.h"
#include "Source.h"
#include "QF.h"

#include <atlbase.h>

Point3 Source::getOrigin() const
{
	return origin_;
}

Point3 Source::getNormal() const
{
	return normal_;
}

Point3 Source::localCoord(const Point2& p) const
{
	Point3 lp(p.get<0>(), p.get<1>(), zLevel());

	return lp;
}

void Source::prepare() const
{ }

void FakeSource::fillNode(Node& node) const
{
	// just emulator
	auto p = localCoord(node);
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

Point3 OrthoPlaneSource::localCoord(const Point2& p) const
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

void OrthoPlaneSource::prepare() const
{ 

}

const std::string RandD{ "E:\\R&D\\" };			// devDell
//const std::string RandD{ "E:\\R&D\\" };		// Dev10
//const std::string RandD{ "D:\\R&D\\" };		// simon-think
const std::string problemPath = RandD +
	"Дмитрий Влад. Кузнецов\\2021-July_ИмпульсПроект\\_PART-2 (ПС-110 здание)\\Problems";
	//"Дмитрий Влад. Кузнецов\\2019-July-01_ИмпульсПроект\\ВЛ-750\\Problems";

	complex_vector toComplexVector(ComplexVectorPtr& cv)
{
	double reX = cv->Re->X;
	double reY = cv->Re->Y;
	double imX = cv->Im->X;
	double imY = cv->Im->Y;
	return complex_vector(cv->Re->X, cv->Re->Y, cv->Im->X, cv->Im->Y);
}

void OrthoPlaneSource::fillNode(Node& node) const
{
	auto lp = localCoord(node);
	double x = lp.get<0>();
	double y = lp.get<1>();
	double z = lp.get<2>();

	if (isInsideBox(Point2(x, z)))
	{
		QF()->DefaultFilePath = _bstr_t(problemPath.c_str());
		assert(!problemName_.empty());
		IProblemPtr pbm = QF()->Problems->Open(problemName_.c_str());

		if (!pbm->Solved)
			pbm->SolveProblem(FALSE);
		pbm->AnalyzeResults();

		IResultPtr res = pbm->Result;

		field_t u(0, 0);
		complex_vector grad;

		if (pbm->ProblemType == qfTimeHarmonicElectric)
		{
			FieldPointECPtr field = res->GetLocalValues(QF()->PointXY(x, y));
			u = field_t(field->U->Re, field->U->Im);
			grad = toComplexVector(field->E);
		}
		else if (pbm->ProblemType == qfTimeHarmonicMagnetics)
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
