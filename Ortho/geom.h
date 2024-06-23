#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

using namespace boost::geometry;

using Point2 = model::point<double, 2, cs::cartesian>;
using Point3 = model::point<double, 3, cs::cartesian>;
using Box3 = model::box<Point3>;

class Box2 : public model::box<Point2>
{
public:
	Box2(double minX, double maxX, double minY, double maxY) :
		model::box<Point2>(Point2(minX, minY), Point2(maxX, maxY))

	{ 	}
};

double smul(const Point2& a, const Point2& b);
double vmul(const Point2& a, const Point2& b);
double angle(const Point2& a, const Point2& b);	// угол между двумя векторами

