#include "stdafx.h"
#include "iostream"
#include "Data.h"
#include "Source.h"
#include "Summator.h"
#include "geom.h"
#include "field.h"

int main(size_t argc, char* argv[])
{
	std::cout << "Ortho called with args:";
	for (size_t i = 1; i < argc; i++)
		std::cout << argv[i] << " ";

	Data data(argc, argv);
	Summator solver(data.bounds(), data.divx(), data.divy());
	solver.run(data.sources());
	solver.save("grid.txt");

	std::cout << "\n\n Press any key to exit";
	char c;
	std::cin >> c;
}



