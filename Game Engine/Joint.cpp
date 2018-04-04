#include "Joint.h"

Joint::Joint()
{
}

Joint::~Joint()
{
}

bool Joint::operator==(const Joint & other)
{
	bool isEqual = false;

	if (this->parentIndex == other.parentIndex && this->name == other.name && this->parentName == other.parentName && this->offsetMatrix == other.offsetMatrix
		&& this->transformMatrix == other.transformMatrix && this->localTransform == other.localTransform && this->nodeTransform == other.nodeTransform &&
		this->rot == other.rot && this->pos == other.pos && this->scale == other.scale && this->childrenNameVector == other.childrenNameVector
		&& this->childrenIDVector == other.childrenIDVector && this->posVector == other.posVector && this->rotVector == other.rotVector && this->scaleVector == other.scaleVector
		&& this->posTimes == other.posTimes && this->rotTimes == other.rotTimes && this->scaleTimes == other.scaleTimes) {
		isEqual = true;
	}

	return isEqual;
}
