#pragma once
#include<string>

class GameObject;

class Component
{
public:
	Component();
	virtual ~Component();
    GameObject* gameObject;
	std::string assetName;
	bool operator==(const Component &rhs);
	virtual void setInformation(float time);
	virtual void update(float deltaTime);

private:

};
