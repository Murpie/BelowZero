#include "SkelletonVertex.h"

SkelletonVertex::SkelletonVertex()
{
}

SkelletonVertex::~SkelletonVertex()
{
}

bool SkelletonVertex::operator==(const SkelletonVertex & other)
{
	bool isEqual = false;

	if (this->x == other.x && this->y == other.y && this->z == other.z && this->u == other.u && this->v == other.v && this->nx == other.nx && this->ny == other.ny && this->nz == other.z
		&& this->weight1 == other.weight1 && this->weight2 == other.weight2 && this->weight3 == other.weight3 && this->weight4 == other.weight4 && this->ID1 == other.ID1 && this->ID2 == other.ID2&& this->ID3 == other.ID3 && this->ID4 == other.ID4) {
		isEqual = true;
	}

	return isEqual;
}
