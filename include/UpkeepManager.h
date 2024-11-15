#pragma once
#include "boost_unordered.hpp"

// Class unused, must be enabled in PCH.h
class UpkeepManager
{
public:
	[[nodiscard]] static UpkeepManager* GetSingleton()
	{
		static UpkeepManager singleton;
		return std::addressof(singleton);
	}

	bool InsertLookupItem(RE::FormID list, RE::FormID item);
	bool InsertLookupBatch(RE::TESLevItem* list);
	bool CheckOutfit(RE::Character* character);

	// key is generated leveled list and value is all of the contents of the generated level list
	//boost::unordered_flat_map<RE::TESLevItem*, boost::unordered_flat_set<RE::TESForm*>> itemOutfitLookupMap;
	boost::unordered_flat_map<RE::FormID, boost::unordered_flat_set<RE::FormID>> itemOutfitLookupMap;

	size_t totalOutfitItems = 0;
};