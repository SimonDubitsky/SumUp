#pragma once
#include "geom.h"
#include "field.h"
#include <vector>
#include <string>

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

	virtual Point3 getOrigin() const;	// location of local CS in the global
	virtual Point3 getNormal() const;	// location to the plane of local CS in the global
	virtual Point2 localCoord(const Point2& p) const;	// global -> local conversion
	virtual void fillNode(Node& node) const = 0; // calculate field in the given point

	virtual double zLevel() const { return zLevel_; }
	virtual void zLevel(double z) { zLevel_ = z; }
	virtual void prepare();

private:
	Point3 origin_;	// положение начала координат источника в глобальной системе
	Point3 normal_;	// ориентация нормали к плоскости источника в глобальной системе
	double zLevel_; // высота над землей
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
	OrthoPlaneSource(const std::string& problemName, 
		const Point3& origin, const Point3& normal,
		double minX, double maxX, double minY, double maxY);
	Point2 localCoord(const Point2& p) const override;
	void prepare() override;
	void fillNode(Node& node) const override;

	bool insideBox(const Point2& p) const;

private:
	const std::string& problemName_;
	const Box2 box_;	// в осях X-Z локальной системы координат
};
