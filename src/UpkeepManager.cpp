#include "UpkeepManager.h"
#include "Data.h"
#include <vector>

// Class unused, must be enabled in PCH.h

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

bool UpkeepManager::CheckOutfit(RE::Character* character)
{
	if (character && (!character->IsDead()))
	{
		std::vector<boost::unordered_flat_set<RE::FormID>*> generatedLeveledListItems;

		size_t itemsInExtraOutfitCount = 0;
		size_t generatedLeveledListsDetected;

		// check original outfit form for generated leveled lists
		auto actorBase = character->GetActorBase();

		if (actorBase)
		{
			if (actorBase->defaultOutfit)
			{
				auto& outfitItems = actorBase->defaultOutfit->outfitItems;
				size_t outfitItemsSize = outfitItems.size();
				
				for (size_t i = 0; i < outfitItemsSize; ++i)
				{
					if (outfitItems[i])
					{
						if (auto mapIterator = itemOutfitLookupMap.find(outfitItems[i]->formID); mapIterator != itemOutfitLookupMap.end())
						{
							// ExtraOutfitItem also contains a FormID to its BGSOutfit, use instead of ActorBase?
							generatedLeveledListItems.push_back(&(mapIterator->second));
						}
					}
				}
			}
		}

		generatedLeveledListsDetected = generatedLeveledListItems.size();

		// check outfit extra data (outfit in inventory / equipped)
		if (generatedLeveledListsDetected)
		{
			auto inventoryChanges = character->GetInventoryChanges();

			if (inventoryChanges)
			{
				auto& entryList = inventoryChanges->entryList;
				auto listIterator = entryList->begin();

				while (listIterator != entryList->end())
				{
					if ((*listIterator))
					{
						auto extraLists = (*listIterator)->extraLists;

						if (extraLists)
						{
							auto extraIterator = extraLists->begin();

							while (extraIterator != extraLists->end())
							{
								if ((*extraIterator))
								{
									auto dataIterator = (*extraIterator)->begin();

									while (dataIterator != (*extraIterator)->end())
									{
										if (dataIterator->GetType() == RE::ExtraDataType::kOutfitItem)
										{
											for (size_t k = 0; k < generatedLeveledListsDetected; ++k)
											{
												// add countDelta check on InventoryEntryData ((*listIterator)) compared to character->GetContainer()'s count?
												if ((*listIterator)->object && generatedLeveledListItems[k]->count((*listIterator)->object->formID))
												{
													++itemsInExtraOutfitCount;
												}
											}
										}

										++dataIterator;
									}
								}

								++extraIterator;
							}
						}
					}

					++listIterator;
				}
			}
		}

		// make npcs reequip outfit items if not already wearing
		if (generatedLeveledListsDetected != itemsInExtraOutfitCount)
		{
			character->RemoveOutfitItems(character->GetActorBase()->defaultOutfit);
			character->AddWornOutfit(character->GetActorBase()->defaultOutfit, false);

			character->RemoveOutfitItems(character->GetActorBase()->sleepOutfit);
			character->AddWornOutfit(character->GetActorBase()->sleepOutfit, false);

			character->InitInventoryIfRequired(false);

			auto inventoryChanges = character->GetInventoryChanges();
			const auto actorEquipManager = RE::ActorEquipManager::GetSingleton();

			if (inventoryChanges)
			{
				auto& entryList = inventoryChanges->entryList;
				auto listIterator = entryList->begin();

				while (listIterator != entryList->end())
				{
					if ((*listIterator))
					{
						auto extraLists = (*listIterator)->extraLists;

						if (extraLists)
						{
							auto extraIterator = extraLists->begin();

							while (extraIterator != extraLists->end())
							{
								if ((*extraIterator))
								{
									auto dataIterator = (*extraIterator)->begin();

									while (dataIterator != (*extraIterator)->end())
									{
										if (dataIterator->GetType() == RE::ExtraDataType::kOutfitItem)
										{
											auto armor = (*listIterator)->object ? (*listIterator)->object->As<RE::TESObjectARMO>() : nullptr;

											if (armor)
											{
												actorEquipManager->EquipObject(character, armor, nullptr, 1, armor->GetEquipSlot(), true, false, true, true);
											}
										}

										++dataIterator;
									}
								}

								++extraIterator;
							}
						}
					}

					++listIterator;
				}
			}
		}
	}

	return false;
}