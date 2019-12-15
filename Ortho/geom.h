#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

using namespace boost::geometry;

using Point2 = model::point<double, 2, cs::cartesian>;
using Point3 = model::point<double, 3, cs::cartesian>;
using Box2 = model::box<Point2>;
using Box3 = model::box<Point3>;
