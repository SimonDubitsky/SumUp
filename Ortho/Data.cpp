#include "stdafx.h"
#include "Data.h"

Data::Data(size_t argc, char* argv[])
{
	sources_.push_back(new OrthoPlaneSource("1-ES.pbm", Point3(60.3, 228, 0), Point3(0, -1, 0), 
		-100, 100, -50, 188.5));
	sources_.push_back(new OrthoPlaneSource("2-ES.pbm", Point3(128.5, 228, 0), Point3(0, -1, 0),
		-100, 100, -50, 55));
	sources_.push_back(new OrthoPlaneSource("3-ES.pbm", Point3(254.350, 228, 0), Point3(0, -1, 0),
		-100, 100, -50, 188.5));
	sources_.push_back(new OrthoPlaneSource("4-ES.pbm", Point3(43, 180, 0), Point3(1, 0, 0),
		-100, 100, -43, 223));
	sources_.push_back(new OrthoPlaneSource("5-ES.pbm", Point3(43, 132.5, 0), Point3(1, 0, 0),
		-100, 100, 0, 95.5));
	sources_.push_back(new OrthoPlaneSource("6-ES.pbm", Point3(606, 132.5, 0), Point3(1, 0, 0),
		-100, 100, -211, 60));
	sources_.push_back(new OrthoPlaneSource("7-ES.pbm", Point3(606, 50, 0), Point3(1, 0, 0),
		-100, 100, -124, 60));
	sources_.push_back(new OrthoPlaneSource("8-ES.pbm", Point3(43, 180, 0), Point3(1, 0, 0),
		-100, 100, 0, 230));
	sources_.push_back(new OrthoPlaneSource("9-ES.pbm", Point3(43, 132.5, 0), Point3(1, 0, 0),
		-100, 100, -37, 0));
	sources_.push_back(new OrthoPlaneSource("10-ES.pbm", Point3(138, 132.5, 0), Point3(1, 0, 0),
		-100, 100, -25, 128));
	sources_.push_back(new OrthoPlaneSource("11-ES.pbm", Point3(122, 101, 0), Point3(1, 0, 0),
		-100, 100, -30, 0));
	sources_.push_back(new OrthoPlaneSource("12-ES.pbm", Point3(253, 92, 0), Point3(1, 0, 0),
		-100, 100, -30, 0));
	sources_.push_back(new OrthoPlaneSource("13-ES.pbm", Point3(97.2, 100, 0), Point3(1, 0, 0),
		-100, 100, 0, 141.5));

	for (auto s : sources_)
		s->zLevel(zLevel());
}

Box2 Data::bounds() const
{
	return Box2(0, 375, 0, 290);
}

size_t Data::divx() const
{
	return 20;
}

size_t Data::divy() const
{
	return 20;
}

double Data::zLevel() const
{
	return 1.8;
}


const Data::Sources& Data::sources() const
{
	return sources_;
}

