#pragma once

#include <vector>

template <typename T>
class ExcludeCollection
{
	public:
	RE::FormID parentFormID{};
	T parentPointer{};

	ExcludeCollection(const ExcludeCollection& other)
	{
		parentFormID = other.parentFormID;
		parentPointer = other.parentPointer;
	}

	ExcludeCollection(const RE::FormID formID, const T pointer)
	{
		parentFormID = formID;
		parentPointer = pointer;
	}

	bool operator==(const ExcludeCollection& other) const
	{
		return parentFormID == other.parentFormID;
	}
};

namespace boost
{
	template<typename T>
	struct hash<ExcludeCollection<T>>
	{
		std::size_t operator()(const ExcludeCollection<T>& obj) const
		{
			return obj.parentFormID;
		}
	};
}