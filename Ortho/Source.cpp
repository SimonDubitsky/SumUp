#include "stdafx.h"
#include "Source.h"

Point3 Source::getOrigin() const
{
	return origin_;
}

Point3 Source::getNormal() const
{
	return normal_;
}

Point2 Source::localCoord(const Point2& p) const
{
	Point2 lp(p);

	return lp;
}

void Source::prepare()	
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

Point2 OrthoPlaneSource::localCoord(const Point2& p) const
{
	Point2 lp(p);

	// сдвиг
	lp.set<0>(p.get<0>() - getOrigin().get<0>());
	lp.set<1>(p.get<1>() - getOrigin().get<1>());

	// поворот относительно оси Z
	double angle = atan2(getNormal().get<1>(), getNormal().get<0>());
	double si = sin(angle);
	double co = cos(angle);
	lp.set<0>(lp.get<0>() * co - lp.get<1>() * si);
	lp.set<1>(lp.get<0>() * si + lp.get<1>() * co);

	return lp;
}

bool OrthoPlaneSource::insideBox(const Point2& p) const
{
	return box_.min_corner().get<0>() <= p.get<0>() && p.get<0>() <= box_.max_corner().get<0>()
		&& box_.min_corner().get<1>() <= p.get<1>() && p.get<1>() <= box_.max_corner().get<1>();
}

void OrthoPlaneSource::::prepare()
{ 
	assert(!problemName_.empty());


}

void OrthoPlaneSource::fillNode(Node& node) const
{
	// just emulator
	auto lp = localCoord(node);
	double x = lp.get<0>();
	double y = lp.get<1>();

	if (insideBox(lp))
	{
		field_t u(0, 0);
		complex_vector grad{ 0, 0, 0, 0 };

		node.u += u;
		node.grad += grad;
	}
}
