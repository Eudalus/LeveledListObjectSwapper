#pragma once

class SmallerContainerObject
{
public:
	std::int32_t count;
	RE::TESBoundObject* obj;

	SmallerContainerObject(){}

	SmallerContainerObject(const RE::ContainerObject& object)
	{
		count = object.count;
		obj = object.obj;
	}

	SmallerContainerObject(int32_t number, RE::TESBoundObject* object)
	{
		count = number;
		obj = object;
	}
};