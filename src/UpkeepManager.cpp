#include "UpkeepManager.h"
#include "Data.h"

bool UpkeepManager::InsertLookupItem(RE::FormID list, RE::FormID item)
{
	if (auto mapIterator = itemOutfitLookupMap.find(list); mapIterator != itemOutfitLookupMap.end())
	{
		//if (mapIterator->second.size() < Data::MAX_ENTRY_SIZE) // maybe unnecessary check
		//{
		mapIterator->second.emplace(item);
		++totalOutfitItems;
		return true;
		//}
	}
	else
	{
		itemOutfitLookupMap.emplace(list, boost::unordered_flat_set<RE::FormID>());
		itemOutfitLookupMap.at(list).emplace(item);
		++totalOutfitItems;

		return true;
	}

	return false;
}

bool UpkeepManager::InsertLookupBatch(RE::TESLevItem* list)
{
	if (list)
	{
		size_t size = std::min((size_t)list->numEntries, list->entries.size());

		if (auto mapIterator = itemOutfitLookupMap.find(list->formID); mapIterator != itemOutfitLookupMap.end())
		{
			mapIterator->second.reserve(size);

			for (size_t i = 0; i < size; ++i)
			{
				if (list->entries[i].form)
				{
					mapIterator->second.emplace(list->entries[i].form->formID);
					++totalOutfitItems;
				}
			}
		}
		else
		{
			itemOutfitLookupMap.emplace(list->formID, boost::unordered_flat_set<RE::FormID>());
			
			if (mapIterator = itemOutfitLookupMap.find(list->formID); mapIterator != itemOutfitLookupMap.end()) // mapIterator declared in first if statement
			{
				mapIterator->second.reserve(size);

				for (size_t i = 0; i < size; ++i)
				{
					if (list->entries[i].form)
					{
						mapIterator->second.emplace(list->entries[i].form->formID);
						++totalOutfitItems;
					}
				}
			}
		}

		return true;
	}

	return false;
}

bool UpkeepManager::ProcessOutfit(RE::Character* character)
{
	if (character)
	{

	}

	return false;
}