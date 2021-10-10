#pragma once
#include "geom.h"
#include "Source.h"
#include <vector>

class Data
{
public:
	Data(size_t argc, char* argv[]);

	using Sources = std::vector<Source*>;

	// размер главной плоскости 
	const Box2& bounds() const;
	Box2& bounds();
	void setBounds(const Box2& box);
	
	// количество разбиений по осям X и Y
	unsigned divx() const { return divx_;  }
	void setDivx(int ix) { divx_ = ix;  }
	unsigned divy() const { return divy_; }
	void setDivy(int iy) { divy_ = iy; }

	// высота плоскости, в котороый вычисляется картина поля
	double zLevel() const { return zLevel_; }
	void setZLevel(double z) { zLevel_ = z; }
	// список задач-источников
	const Sources& sources() const;
	Sources& sources();

private:
	Sources sources_;
	unsigned divx_ = 40;
	unsigned divy_ = 40;
	double zLevel_ = 1.8;
	Box2 box_;
};

