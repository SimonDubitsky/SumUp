#include "stdafx.h"
#include "Data.h"

Data::Data(size_t argc, char* argv[])
{
	// Electric field
	sources_.push_back(new OrthoPlaneSource("1_ES.pbm", Point3(60.3, 228, 0), Point3(0, -1, 0), 
		-100, 100, -50, 188.5));
	sources_.push_back(new OrthoPlaneSource("2_ES.pbm", Point3(128.5, 228, 0), Point3(0, -1, 0),
		-100, 100, -50, 55));
	sources_.push_back(new OrthoPlaneSource("3_ES.pbm", Point3(254.350, 228, 0), Point3(0, -1, 0),
		-100, 100, -50, 188.5));
	sources_.push_back(new OrthoPlaneSource("4_ES.pbm", Point3(43, 180, 0), Point3(1, 0, 0),
		-100, 100, -43, 223));
	sources_.push_back(new OrthoPlaneSource("5_ES.pbm", Point3(43, 132.5, 0), Point3(1, 0, 0),
		-100, 100, 0, 95.5));
	sources_.push_back(new OrthoPlaneSource("6_ES.pbm", Point3(606, 132.5, 0), Point3(1, 0, 0),
		-100, 100, -211, 60));
	sources_.push_back(new OrthoPlaneSource("7_ES.pbm", Point3(606, 50, 0), Point3(1, 0, 0),
		-100, 100, -124, 60));
	sources_.push_back(new OrthoPlaneSource("8_ES.pbm", Point3(43, 180, 0), Point3(1, 0, 0),
		-100, 100, 0, 230));
	sources_.push_back(new OrthoPlaneSource("9_ES.pbm", Point3(43, 132.5, 0), Point3(1, 0, 0),
		-100, 100, -37, 0));
	sources_.push_back(new OrthoPlaneSource("10_ES.pbm", Point3(138, 132.5, 0), Point3(1, 0, 0),
		-100, 100, -25, 128));
	sources_.push_back(new OrthoPlaneSource("11_ES.pbm", Point3(122, 101, 0), Point3(1, 0, 0),
		-100, 100, -30, 0));
	sources_.push_back(new OrthoPlaneSource("12_ES.pbm", Point3(253, 92, 0), Point3(1, 0, 0),
		-100, 100, -30, 0));
	sources_.push_back(new OrthoPlaneSource("13_ES.pbm", Point3(97.2, 100, 0), Point3(1, 0, 0),
		-100, 100, 0, 141.5));

	// Magnetic field
	//sources_.push_back(new OrthoPlaneSource("1_HE.pbm", Point3(60.3, 228, 0), Point3(0, -1, 0),
	//	-100, 100, -50, 188.5));
	//sources_.push_back(new OrthoPlaneSource("2_HE.pbm", Point3(128.5, 228, 0), Point3(0, -1, 0),
	//	-100, 100, -50, 55));
	//sources_.push_back(new OrthoPlaneSource("3_HE.pbm", Point3(254.350, 228, 0), Point3(0, -1, 0),
	//	-100, 100, -50, 188.5));
	//sources_.push_back(new OrthoPlaneSource("4_HE.pbm", Point3(43, 180, 0), Point3(1, 0, 0),
	//	-100, 100, -43, 223));
	//sources_.push_back(new OrthoPlaneSource("5_HE.pbm", Point3(43, 132.5, 0), Point3(1, 0, 0),
	//	-100, 100, 0, 95.5));
	//sources_.push_back(new OrthoPlaneSource("6_HE.pbm", Point3(606, 132.5, 0), Point3(1, 0, 0),
	//	-100, 100, -211, 60));
	//sources_.push_back(new OrthoPlaneSource("7_HE.pbm", Point3(606, 50, 0), Point3(1, 0, 0),
	//	-100, 100, -124, 60));
	//sources_.push_back(new OrthoPlaneSource("8_HE.pbm", Point3(43, 180, 0), Point3(1, 0, 0),
	//	-100, 100, 0, 230));
	//sources_.push_back(new OrthoPlaneSource("9_HE.pbm", Point3(43, 132.5, 0), Point3(1, 0, 0),
	//	-100, 100, -37, 0));
	//sources_.push_back(new OrthoPlaneSource("10_HE.pbm", Point3(138, 132.5, 0), Point3(1, 0, 0),
	//	-100, 100, -25, 128));
	//sources_.push_back(new OrthoPlaneSource("11_HE.pbm", Point3(122, 101, 0), Point3(1, 0, 0),
	//	-100, 100, -30, 0));
	//sources_.push_back(new OrthoPlaneSource("12_HE.pbm", Point3(253, 92, 0), Point3(1, 0, 0),
	//	-100, 100, -30, 0));
	//sources_.push_back(new OrthoPlaneSource("13_HE.pbm", Point3(97.2, 100, 0), Point3(1, 0, 0),
	//	-100, 100, 0, 141.5));

	for (auto s : sources_)
		s->zLevel(zLevel());
}

Box2 Data::bounds() const
{
	return Box2(0, 375, 0, 290);
}

size_t Data::divx() const
{
	return 40;
}

size_t Data::divy() const
{
	return 40;
}

double Data::zLevel() const
{
	return 1.8;
}


const Data::Sources& Data::sources() const
{
	return sources_;
}

