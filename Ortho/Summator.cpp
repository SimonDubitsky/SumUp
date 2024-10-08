#include "stdafx.h"
#include "Summator.h"
#include <fstream>
#include <iostream>
#include <chrono>

Node::Node(double x_, double y_) : 
	Point2(x_, y_),
	u(0),
	Bre(0, 0, 0), Bim(0, 0, 0)
{ }

Node::Node(const Point2& point) :
	Point2(point),
	u(0),
	Bre(0, 0, 0), Bim(0, 0, 0)
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
	using namespace std::literals; // enables literal suffixes, e.g. 24h, 1ms, 1s.

	for (auto* source : sources)
	{
		auto start = std::chrono::steady_clock::now();
		std::cout << std::endl << "  name: " << source->name();
		source->prepare();

		for (auto& row : rows_)
		{	
			for (auto& node : row)
			{
				source->fillNode(node);
			}
		}
		auto end = std::chrono::steady_clock::now();
		source->release();
		std::cout << " duration: " << (end - start) / 1s << " s.";
	}
}

void Summator::save(const std::string& filename)
{
	std::ofstream stream(filename, std::ios::out);
	const std::string dlm("; ");
	
	// table caption
	stream << "x" << dlm << "y" << dlm << "potential" << dlm << "grad" << dlm << "B3d" << std::endl;

	int counter = 0;
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

			double uRMS = abs(node.u) / sqrt(2);

			double reX = node.grad.real().x();
			double reY = node.grad.real().y();
			double imX = node.grad.imag().x();
			double imY = node.grad.imag().y();
			double gradRMS = sqrt(reX * reX + reY * reY + imX * imX + imY *imY) / sqrt(2);

			double BreX = node.Bre.get<0>(), BreY = node.Bre.get<1>(), BreZ = node.Bre.get<2>();
			double BimX = node.Bim.get<0>(), BimY = node.Bim.get<1>(), BimZ = node.Bim.get<2>();
			double Brms = sqrt(BreX * BreX + BreY * BreY + BreZ * BreZ +
				BimX * BimX + BimY * BimY + BimZ * BimZ);

			stream << node.get<0>() << dlm << node.get<1>() << dlm
				<< uRMS << dlm << gradRMS << dlm
				<< Brms << dlm
				<< std::endl;
			counter++;
		}
	}

	std::cout << std::endl << " " << counter << " nodes saved to '" << filename << "' file.";
}
