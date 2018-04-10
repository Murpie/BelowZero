#include "Component.h"
#include "GameObject.h"

Component::Component()
{

}

Component::~Component()
{

}

bool Component::operator==(const Component & rhs)
{
	bool value = false;

	if (this->assetName == rhs.assetName) {
		value = true;
	}
	return value;
}

void Component::setInformation(float time)
{

}

void Component::update(float deltaTime)
{

}

