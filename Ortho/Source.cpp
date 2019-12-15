#include "stdafx.h"
#include "Source.h"

Point3 Source::getOrigin() const
{
	return origin_;
}

Point3 Source::getNormal() const
{
	return normal_;
}

void OrthoPlaneSource::fillNode(Node& node) const
{

}
