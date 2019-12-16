#include "stdafx.h"
#include "Summator.h"
#include <fstream>

Node::Node(double x_, double y_) : 
	Point2(x_, y_),
	u(0)
{ }

Node::Node(const Point2& point) :
	Point2(point),
	u(0)
{ }


Summator::Summator(Box2 box, size_t nx, size_t ny) :
	box_(box)
{
	assert(nx > 0 && ny > 0);

	double width = box_.max_corner().get<0>() - box_.min_corner().get<0>();
	double height = box_.max_corner().get<1>() - box_.min_corner().get<1>();
	assert(height > 0 && width > 0);

	d_ = Point2(width / nx, height / ny);
	double y = 0;
	for (size_t irow = 0; irow <= ny; irow++)
	{
		double x = 0;
		rows_.push_back(Row());
		auto& row = rows_.back();
		for (size_t icol = 0; icol <= nx; icol++)
		{
			row.push_back(Node(x, y));
			x += d_.get<0>();
		}
		y += d_.get<1>();
	}
}

void Summator::run(const Data::Sources& sources)
{
	for (const auto* source : sources)
	{
		source->prepare();
		for (auto& row : rows_)
		{	
			for (auto& node : row)
			{
				source->fillNode(node);
			}
		}
	}
}

void Summator::save(const std::string& filename)
{
	std::ofstream stream(filename, std::ios::out);
	const std::string dlm("; ");

	for (auto row : rows_)
	{
		for (auto node : row)
		{
			stream /*<< std::setw(20)*/
				<< node.get<0>() << dlm << node.get<1>() << dlm
				<< node.u.real() << dlm << node.u.imag() << dlm
				<< node.grad.real().get<0>() << dlm << node.grad.real().get<1>() << dlm
				<< node.grad.imag().get<0>() << dlm << node.grad.imag().get<1>() << dlm
				<< std::endl;
		}
	}
}
