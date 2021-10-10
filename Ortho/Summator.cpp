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

	double xmin = box_.min_corner().get<0>();
	double xmax = box_.max_corner().get<0>();
	double ymin = box_.min_corner().get<1>();
	double ymax = box_.max_corner().get<1>();

	double width = xmax - xmin;
	double height = ymax - ymin;
	assert(height > 0 && width > 0);

	d_ = Point2(width / nx, height / ny);
	double y = ymin;
	for (size_t irow = 0; irow <= ny; irow++)
	{
		double x = xmin;
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
	
	// table caption
	stream << "x" << dlm << "y" << dlm << "potential" << dlm << "grad" << dlm << std::endl;

	for (auto row : rows_)
	{
		for (auto node : row)
		{
			//stream /*<< std::setw(20)*/
			//	<< node.get<0>() << dlm << node.get<1>() << dlm
			//	<< node.u.real() << dlm << node.u.imag() << dlm
			//	<< node.grad.real().get<0>() << dlm << node.grad.real().get<1>() << dlm
			//	<< node.grad.imag().get<0>() << dlm << node.grad.imag().get<1>() << dlm
			//	<< std::endl;

			double uRMS = abs(node.u);
			double reX = node.grad.real().x();
			double reY = node.grad.real().y();
			double imX = node.grad.imag().x();
			double imY = node.grad.imag().y();

			double gradRMS = sqrt(reX * reX + reY * reY + imX * imX + imY *imY) / sqrt(2);

			stream << node.get<0>() << dlm << node.get<1>() << dlm
				<< uRMS << dlm << gradRMS << dlm
				<< std::endl;
		}
	}
}
