#pragma once
#include "geom.h"
#include "field.h"
#include "QF.h"

#include <vector>
#include <string>
#include <numbers>

const double mu0 = 4 * std::numbers::pi * 1e-7;
const double eps0 = 8.85418782e-12;

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
	virtual Point3 globalCoord(const Point2& p) const;	// local -> global conversion
	virtual void fillNode(Node& node) const = 0; // calculate field in the given point

	virtual double zLevel() const { return zLevel_; }
	virtual void zLevel(double z) { zLevel_ = z; }
	virtual void prepare();
	virtual void release();
	virtual std::string name() const;

private:
	Point3 origin_;	// ��������� ������ ��������� ��������� � ���������� �������
	Point3 normal_;	// ���������� ������� � ��������� ��������� � ���������� �������
	double zLevel_; // ������ ��� ������
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
	OrthoPlaneSource(const std::string& problemName,
		const Point3& origin, const Point3& normal, const Box2& box);

	Point3 globalCoord(const Point2& p) const override;
	void prepare() override;
	void release() override;
	std::string name() const override;
	void fillNode(Node& node) const override;

	std::string problemName() const { return problemName_; }
	const Box2& box() const { return box_; }

	bool isInsideBox(const Point2& p) const;

private:
	const std::string problemName_;
	const Box2 box_;	// � ���� X-Z ��������� ������� ���������
	IProblemPtr pbm_;
};

// ������ ������ ������� � ����� ��� � �����������
// p1 - ��������� ����� (��� x1, y1, z1)
// p2 - �������� ����� (��� x2, y2, z2)
// problemType  - ��� ������ qfTimeHarmonicMagnetics ��� qfTimeHarmonicElectric
// load - ��������: � ����������� �� ���� ������ ����������� ��� ��� ����������� ���������

class Wire : public Source
{
public:
	Wire(QfProblemTypes problemType, field_t load, const Point3& p1, Point3& p2, const std::string& name = "");
	Wire(QfProblemTypes problemType, field_t load,
		double x1, double y1, double z1,
		double x2, double y2, double z2, const std::string& name = "");

	Point3 globalCoord(const Point2& p) const override;
	void prepare() override;
	std::string name() const override { return name_; }
	void fillNode(Node& node) const override;

private:
	const Point3 start_;
	const Point3 end_;
	QfProblemTypes problemType_;
	field_t load_;
	std::string name_;
};
