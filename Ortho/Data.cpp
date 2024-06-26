#include "stdafx.h"
#include "Data.h"

#include <nlohmann\json.hpp>	// JSON parser/serializer
#include <fstream>

using namespace nlohmann;

namespace
{
	bool fileExists(const std::string& filename)
	{
		if (filename.empty())
			return false;

		std::ifstream stream(filename);
		return stream.is_open();
	}

	void to_json(json& j, const Point3& p3)
	{
		j = json { 
			{"x", p3.get<0>()},
			{"y", p3.get<1>()},
			{"z", p3.get<2>()}
		};
	}

	void from_json(const json& j, Point3& p3)
	{
		p3.set<0>(j["x"]);
		p3.set<1>(j["y"]);
		p3.set<2>(j["z"]);
	}

	void to_json(json& j, const Point2& p2)
	{
		j = json{
			{"x", p2.get<0>()},
			{"y", p2.get<1>()}
		};
	}

	void from_json(const json& j, Point2& p2)
	{
		p2.set<0>(j["x"]);
		p2.set<1>(j["y"]);
	}

	void to_json(json& j, const Box2& b2)
	{
		j = json{
			{"minX", b2.min_corner().get<0>()},
			{"maxX", b2.max_corner().get<0>()},
			{"minY", b2.min_corner().get<1>()},
			{"maxY", b2.max_corner().get<1>()}
		};
	}

	void from_json(const json& j, Box2& b2)
	{
		b2.min_corner().set<0>(j["minX"]);
		b2.max_corner().set<0>(j["maxX"]);
		b2.min_corner().set<1>(j["minY"]);
		b2.max_corner().set<1>(j["maxY"]);
	}

	void saveOrthoPlane(const Data& data, const std::string& filename)
	{
		json j;

		json jZone;
		to_json(jZone, data.bounds());
		jZone["divX"] = data.divx();
		jZone["divY"] = data.divy();
		jZone["zLevel"] = data.zLevel();
		j["zone"] = jZone;

		json jAllSources;
		for (auto source : data.sources())
		{
			auto ortho = static_cast<OrthoPlaneSource*>(source);
			json jSource;

			jSource["problem"] = ortho->problemName();
			to_json(jSource["origin"], ortho->getOrigin());
			to_json(jSource["normal"], ortho->getNormal());
			to_json(jSource["limits"], ortho->box());

			jAllSources.push_back(jSource);
		}
		j["sources"] = jAllSources;

		std::ofstream stream(filename);
		stream << std::setw(2) << j << std::endl;
		stream.flush();
	}

	bool loadOrthoPlane(Data& data, const std::string& filename)
	{
		if (!fileExists(filename))
			return false;

		std::ifstream stream(filename);
		json j;
		stream >> j;
		
		auto jZone = j["zone"];
		from_json(jZone, data.bounds());
		data.setDivx(jZone["divX"]);
		data.setDivy(jZone["divY"]);
		data.setZLevel(jZone["zLevel"]);

		for (auto jSource : j["sources"])
		{
			Point3 origin, normal;
			from_json(jSource["origin"], origin);
			from_json(jSource["normal"], normal);

			Box2 limits(0, 1, 0, 1);
			from_json(jSource["limits"], limits);


			data.sources().push_back(new OrthoPlaneSource(jSource["problem"], origin, normal, limits));
		}

		return true;
	}

}

Data::Data(size_t argc, char* argv[]) :
	box_(0, 0, 0, 0)
{
	if (argc > 1)
	{
		std::string filename = argv[1];
		assert(!filename.empty());

		if (loadOrthoPlane(*this, filename))
			saveOrthoPlane(*this, "input_copy.json");
	}
	else
	{
		// габариты расчетной области
		setBounds(Box2(-10, 30, -20, 20));
		setZLevel(1.8);
		setDivx(160);
		setDivy(160);

		// Отрезки прямых проводов
		// Electric field
		double I = 2000;	// ток 2000 А
		double p120 = 2 * PI / 3;
		double p240 = 4 * PI / 3;

		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, 0),		15, 6, 0.5,		4, 6, 0.5,		"wireA-cable"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, 0),		4, 6, 0.5,		0.9, 6, 2.5,	"wireA-cable_R"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, 0),		0.9, 6, 2.5,	0.9, 6, 5,		"wireA-cable_vert"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, 0),		0.9, 6, 5,		0.9, 6, 10,		"wireA-joint"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, 0),		0.9, 6, 10,		5.3, 6, 21.8,	"wireA-slope"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, 0),		5.3, 6, 21.8,	15, 6, 21.8,	"wireA-overhead"));

		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p120), 15, 0, 0.5, 4, 0, 0.5, "wireB-cable"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p120), 4, 0, 0.5, 0.9, 0, 2.5, "wireB-cable_R"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p120), 0.9, 0, 2.5, 0.9, 0, 5, "wireB-cable_vert"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p120), 0.9, 0, 5, 0.9, 0, 10, "wireB-joint"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p120), 0.9, 0, 10, 5.3, 0, 21.8, "wireB-slope"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p120), 5.3, 0, 21.8, 15, 0, 21.8, "wireB-overhead"));

		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p240), 15, -6, 0.5, 4, -6, 0.5, "wireC-cable"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p240), 4, -6, 0.5, 0.9, -6, 2.5, "wireC-cable_R"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p240), 0.9, -6, 2.5, 0.9, -6, 5, "wireC-cable_vert"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p240), 0.9, -6, 5, 0.9, -6, 10, "wireC-joint"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p240), 0.9, -6, 10, 5.3, -6, 21.8, "wireC-slope"));
		sources_.push_back(new Wire(qfTimeHarmonicMagnetics, field_t(I, p240), 5.3, -6, 21.8, 15, -6, 21.8, "wireC-overhead"));

		// Electric field
		//sources_.push_back(new OrthoPlaneSource("1_ES.pbm", Point3(60.3, 228, 0), Point3(0, -1, 0),
		//	-100, 100, -50, 188.5));
		//sources_.push_back(new OrthoPlaneSource("2_ES.pbm", Point3(128.5, 228, 0), Point3(0, -1, 0),
		//	-100, 100, -50, 55));
		//sources_.push_back(new OrthoPlaneSource("3_ES.pbm", Point3(254.350, 228, 0), Point3(0, -1, 0),
		//	-100, 100, -50, 188.5));
		//sources_.push_back(new OrthoPlaneSource("4_ES.pbm", Point3(43, 180, 0), Point3(1, 0, 0),
		//	-100, 100, -43, 223));
		//sources_.push_back(new OrthoPlaneSource("5_ES.pbm", Point3(43, 132.5, 0), Point3(1, 0, 0),
		//	-100, 100, 0, 95.5));
		//sources_.push_back(new OrthoPlaneSource("6_ES.pbm", Point3(606, 132.5, 0), Point3(1, 0, 0),
		//	-100, 100, -211, 60));
		//sources_.push_back(new OrthoPlaneSource("7_ES.pbm", Point3(606, 50, 0), Point3(1, 0, 0),
		//	-100, 100, -124, 60));
		//sources_.push_back(new OrthoPlaneSource("8_ES.pbm", Point3(43, 180, 0), Point3(1, 0, 0),
		//	-100, 100, 0, 230));
		//sources_.push_back(new OrthoPlaneSource("9_ES.pbm", Point3(43, 132.5, 0), Point3(1, 0, 0),
		//	-100, 100, -37, 0));
		//sources_.push_back(new OrthoPlaneSource("10_ES.pbm", Point3(138, 132.5, 0), Point3(1, 0, 0),
		//	-100, 100, -25, 128));
		//sources_.push_back(new OrthoPlaneSource("11_ES.pbm", Point3(122, 101, 0), Point3(1, 0, 0),
		//	-100, 100, -30, 0));
		//sources_.push_back(new OrthoPlaneSource("12_ES.pbm", Point3(253, 92, 0), Point3(1, 0, 0),
		//	-100, 100, -30, 0));
		//sources_.push_back(new OrthoPlaneSource("13_ES.pbm", Point3(97.2, 100, 0), Point3(1, 0, 0),
		//	-100, 100, 0, 141.5));

		//saveOrthoPlane(*this, "ES_sources.json");

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

		//saveOrthoPlane(*this, "MS_sources.json");
	}

	for (auto s : sources_)
		s->zLevel(zLevel());
	}

const Box2& Data::bounds() const
{
	return box_;
}

Box2& Data::bounds()
{
	return box_;
}

void Data::setBounds(const Box2& box)
{
	box_ = box;
}

const Data::Sources& Data::sources() const
{
	return sources_;
}

Data::Sources& Data::sources()
{
	return sources_;
}
