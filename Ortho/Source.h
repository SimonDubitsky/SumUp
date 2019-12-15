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
	Source(Point3 origin, Point3 normal) :
		origin_(origin), normal_(normal)
	{ }

	virtual Point3 getOrigin() const;
	virtual Point3 getNormal() const;
	virtual void fillNode(Node& node) const = 0;

private:
	Point3 origin_;	// ��������� ������ ��������� ��������� � ���������� �������
	Point3 normal_;	// ���������� ������� � ��������� ��������� � ���������� �������
};

using Sources = std::vector<const Source*>;

class OrthoPlaneSource : public Source
{
	void fillNode(Node& node) const override;
};