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
	virtual void update();
private:
};
