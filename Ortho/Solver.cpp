#include "stdafx.h"
#include "Solver.h"

Solver::Solver(Box2 box, size_t nx, size_t ny) :
	box_(box)
{
	assert(nx > 0 && ny > 0);

	double width = box_.max_corner().get<0>() - box_.min_corner().get<0>();
	double height = box_.max_corner().get<1>() - box_.min_corner().get<1>();
	assert(height > 0 && width > 0);

	d_ = Point2(width / nx, height / ny);
	for (size_t irow = 0; irow <= ny; irow++)
		rows_.push_back(Row(nx + 1));
}

void Solver::run(const Data::Sources& sources)
{
	for (const auto* source : sources)
	{
		for (auto row : rows_)
		{	
			for (auto grad : row)
			{
				grad += source->getGrad();
			}
		}
	}
}

void Solver::save(const std::string& filname)
{

}
