#pragma once
#include "geom.h"
#include "Source.h"
#include <vector>

class Data
{
public:
	Data(size_t argc, char* argv[]);

	using Sources = std::vector<Source*>;

	Box2 bounds() const;
	size_t divx() const;
	size_t divy() const;
	const Sources& sources() const;

private:
	Sources sources_;
};

