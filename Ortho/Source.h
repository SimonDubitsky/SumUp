#pragma once
#include "geom.h"
#include "field.h"
#include <vector>

class Source
{
public:
	Source(Point3 origin, Point3 normal) :
		origin_(origin), normal_(normal)
	{ }

	virtual Point3 getOrigin() const;
	virtual Point3 getNormal() const;
	virtual complex_vector getGrad() const = 0;

private:
	Point3 origin_;	// положение начала координат источника в глобальной системе
	Point3 normal_;	// ориентация нормали к плоскости источника в глобальной системе
};

using Sources = std::vector<const Source*>;
