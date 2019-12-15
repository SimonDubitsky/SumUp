#pragma once

#include "geom.h"
#include <ccomplex>

using field_t = std::complex<double>;

struct Vector2 : public Point2
{
public:
	Vector2() : 
		Point2(0, 0)
	{ 	}
	
	Vector2(double x, double y) :
		Point2(x, y)
	{ }

	double x() const		{ return get<0>(); }
	double y() const		{ return get<1>(); }

	Vector2 &operator*=(double d) 
	{ 
		set<0>(get<0>() * d);
		set<1>(get<1>() * d);
		return *this;
	}
	Vector2 &operator /= (double d)
	{
		set<0>(get<0>() / d);
		set<1>(get<1>() / d);
		return *this;
	}
	Vector2 &operator+=(const Vector2& v)
	{
		set<0>(get<0>() + v.get<0>());
		set<1>(get<1>() + v.get<1>());
		return *this;
	}
	Vector2 &operator-=(const Vector2& v)
	{
		set<0>(get<0>() - v.get<0>());
		set<1>(get<1>() - v.get<1>());
		return *this;
	}

	Vector2 &operator+(const Vector2& v)
	{
		*this += v;
		return *this;
	}

	Vector2 &operator-(const Vector2& v)
	{
		*this -= v;
		return *this;
	}
};

using complex_vector = std::complex<Vector2>;
