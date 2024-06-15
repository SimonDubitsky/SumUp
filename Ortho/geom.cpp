#include "stdafx.h"
#include "geom.h"

double smul(const Point2& a, const Point2& b)
{
	return a.get<0>() * b.get<0>() + a.get<1>() * b.get<1>();
}

double vmul(const Point2& a, const Point2& b)
{
	return a.get<0>() * b.get<1>() - a.get<1>() * b.get<0>();
}

// угол между двумя векторами
double angle(const Point2& a, const Point2& b)
{
	return atan2(vmul(a, b), smul(a, b));
}
