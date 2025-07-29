#pragma once

#include <vector>

template <typename T>
class ExcludeCollection
{
	public:
	RE::FormID excludeFormID{};
	T excludePointer{};

	ExcludeCollection(const ExcludeCollection& other)
	{
		excludeFormID = other.excludeFormID;
		excludePointer = other.excludePointer;
	}

	ExcludeCollection(const RE::FormID formID, const T pointer)
	{
		excludeFormID = formID;
		excludePointer = pointer;
	}

	bool operator==(const ExcludeCollection& other) const
	{
		return excludeFormID == other.excludeFormID;
	}
};

namespace boost
{
	template<typename T>
	struct hash<ExcludeCollection<T>>
	{
		std::size_t operator()(const ExcludeCollection<T>& obj) const
		{
			return obj.excludeFormID;
		}
	};
}

namespace std
{
	template<typename T>
	struct hash<ExcludeCollection<T>>
	{
		std::size_t operator()(const ExcludeCollection<T>& obj) const
		{
			return obj.excludeFormID;
		}
	};
}