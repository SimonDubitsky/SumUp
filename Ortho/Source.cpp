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

void OrthoPlaneSource::fillNode(Node& node) const
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
