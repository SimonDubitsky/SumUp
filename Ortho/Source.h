#pragma once
#include "geom.h"
#include "field.h"
#include <vector>

// represents a node in the global plane (the target of summation)
// coordinates here are in the global coordinate system
class Node : public Point2
{
public:
	explicit Node(double x = 0, double y = 0);
	Node(const Point2& point);

public:
	field_t u;
	complex_vector grad;
};

class Source
{
public:
	Source() { }
	Source(Point3 origin, Point3 normal) :
		origin_(origin), normal_(normal)
	{ }

	virtual Point3 getOrigin() const;
	virtual Point3 getNormal() const;
	virtual void fillNode(Node& node) const = 0;

	Point2 localCoord(const Point2& p) const;

private:
	Point3 origin_;	// положение начала координат источника в глобальной системе
	Point3 normal_;	// ориентация нормали к плоскости источника в глобальной системе
};

using Sources = std::vector<const Source*>;

class FakeSource : public Source
{
public:
	FakeSource() { }
	void fillNode(Node& node) const override;
};

class OrthoPlaneSource : public Source
{
public:
	OrthoPlaneSource() { }
	void fillNode(Node& node) const override;
};
