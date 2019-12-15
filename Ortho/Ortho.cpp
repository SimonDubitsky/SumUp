#include "stdafx.h"
#include "Data.h"
#include "Source.h"
#include "Summator.h"
#include "geom.h"
#include "field.h"

#include <iostream>
#include <conio.h>

int main(size_t argc, char* argv[])
{
	std::cout << "Ortho called with args:";
	for (size_t i = 1; i < argc; i++)
		std::cout << argv[i] << " ";

	Data data(argc, argv);
	Summator summator(data.bounds(), data.divx(), data.divy());
	summator.run(data.sources());
	summator.save("grid.txt");

	std::cout << "\n\n Press any key to exit. ";
	_kbhit();
	char c = std::cin.get();
}



