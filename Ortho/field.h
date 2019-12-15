#pragma once

#include "geom.h"
#include <ccomplex>

using field_t = std::complex<double>;

constexpr double PI = 3.1415926;

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

class complex_vector : public std::complex<Vector2>
{
public:
	complex_vector()
	{
		real().set<0>(0);
		real().set<1>(0);
		imag().set<0>(0);
		imag().set<1>(0);
	}
	complex_vector(double reX, double reY, double imX, double imY)
	{
		real().set<0>(reX);
		real().set<1>(reY);
		imag().set<0>(imX);
		imag().set<1>(imY);
	}
};