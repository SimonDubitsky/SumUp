#pragma once
#include "geom.h"
#include "field.h"
#include "Source.h"

#include <vector>
#include <string>
#include "Data.h"

class Summator
{
public:
	using Row = std::vector<Node>;	// grid points along x - axis
	using Rows = std::vector<Row>;

	Summator(Box2 box, size_t nx, size_t ny);
	void run(const Data::Sources& souces);
	void save(const std::string& filename);

private:
	Box2 box_;	// size of the output plane
	Point2 d_;	// grid step
	Rows rows_;	// 2d array of field values
};

