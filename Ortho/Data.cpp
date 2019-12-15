#include "stdafx.h"
#include "Data.h"

Data::Data(size_t argc, char* argv[])
{

}

Box2 Data::bounds() const
{
	return Box2(Point2(0, 0), Point2(375000, 290000));
}

size_t Data::divx() const
{
	return 200;
}

size_t Data::divy() const
{
	return 200;
}

const Data::Sources& Data::sources() const
{
	return sources_;
}

