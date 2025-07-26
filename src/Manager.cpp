#include "Manager.h"
#include <SimpleIni.h>
#include "Utility.h"
#include "Data.h"
#include <algorithm>>

#if defined(USING_UPKEEP_MANAGER)
#include "UpkeepManager.h"
#endif

namespace logger = SKSE::log;

bool Manager::LoadData()
{
    logger::info("{:*^30}", "INI");

    std::vector<std::string> configs;
    const std::string DEFAULT_TARGET_STRING;
    const std::string DEFAULT_INSERT_STRING;
    const std::string DEFAULT_COUNT_STRING = "1";
    const std::string DEFAULT_LEVEL_STRING = "1";
    const std::string DEFAULT_CHANCE_STRING = "100.0";
    const std::string DEFAULT_PROTOCOL_STRING = "0";
    const std::string DEFAULT_USE_ALL_STRING = "0";

    const unsigned int DEFAULT_COUNT_VALUE = 1;
    const unsigned int DEFAULT_LEVEL_VALUE = 1;
    const float DEFAULT_CHANCE_VALUE = 100.0;
    const float DEFAULT_CHANCE_MIN = 0.0f;
    const float DEFAULT_CHANCE_MAX = 100.0f;
    const unsigned int DEFAULT_PROTOCOL_VALUE = 0;
    const unsigned int DEFAULT_USE_ALL_VALUE = 0;

    constexpr auto suffix = "_LLOS"sv;

    auto constexpr folder = R"(Data\)";

    ItemData currentData;
    std::string targetString;
    std::string insertString;
    std::string countString;
    std::string levelString;
    std::string chanceString;
    std::string protocolString;
    std::string priorityString;
    std::string useAllString;

    std::string targetEditorID;
    std::string targetPlugin;
    RE::FormID targetFormID;

    std::string insertEditorID;
    std::string insertPlugin;
    RE::FormID insertFormID;

    for (const auto &entry : std::filesystem::directory_iterator(folder))
    {
        if (entry.exists() && !entry.path().empty() && entry.path().extension() == ".ini"sv)
        {
            if (const auto path = entry.path().string(); path.rfind(suffix) != std::string::npos)
            {
                configs.push_back(path);
            }
        }
    }

	if (configs.empty())
    {
        logger::warn("	No .ini files with {} suffix were found within the Data folder, aborting...", suffix);
        return false;
    }

    logger::info("	{} matching inis found", configs.size());

    std::ranges::sort(configs);

    for (auto &path : configs)
    {
        logger::info("		INI : {}", path);

        CSimpleIniA ini;
        ini.SetUnicode();
        ini.SetMultiKey();

        if (const auto rc = ini.LoadFile(path.c_str()); rc < 0)
        {
            logger::error("	couldn't read INI");
            continue;
        }

        CSimpleIniA::TNamesDepend sections;
        ini.GetAllSections(sections);
        sections.sort(CSimpleIniA::Entry::LoadOrder());

        for (auto &[section, comment, order] : sections)
        {
            //logger::info("READ SECTION: {}", section);

            Utility::GetIniValue(ini, targetString, section, "Target",
                              DEFAULT_TARGET_STRING);
            //logger::info("Target: {}", targetString);

            currentData.targetForm = Utility::AcquireForm(targetString, targetEditorID, targetPlugin, targetFormID);

            currentData.targetFormType = Utility::CheckFormType(currentData.targetForm);

            // bypass all data under current section if target is not valid
            if (!currentData.targetForm || currentData.targetFormType == Data::INVALID_FORM_TYPE)
            {
                //logger::warn("error at {} in {}", section, path);
                ++removedDataCounter;
                continue;
            }

            Utility::GetIniValue(ini, insertString, section, "Insert",
                              DEFAULT_INSERT_STRING);
            //logger::info("Insert: {}", insertString);

            currentData.insertForm = Utility::AcquireForm(insertString, insertEditorID, insertPlugin, insertFormID);

            currentData.insertFormType = Utility::CheckFormType(currentData.insertForm);

            Utility::GetIniValue(ini, protocolString, section, "Protocol",
                              DEFAULT_PROTOCOL_STRING);
            //logger::info("Protocol: {}", protocolString);

            //currentData.protocol = Utility::ClampProtocol(Utility::StringToUnInt(protocolString, DEFAULT_PROTOCOL_VALUE));
            currentData.protocol = Utility::StringToUnInt(protocolString, DEFAULT_PROTOCOL_VALUE);

            Utility::GetIniValue(ini, countString, section, "Count", DEFAULT_COUNT_STRING);
            //logger::info("Count: {}", countString);

            // pass by reference assignment
            Utility::AcquireRangeData(countString, currentData.minCount, currentData.maxCount, DEFAULT_COUNT_VALUE);

            Utility::GetIniValue(ini, levelString, section, "Level",
                              DEFAULT_LEVEL_STRING);
            //logger::info("Level: {}", levelString);

            // pass by reference assignment
            Utility::AcquireRangeData(levelString, currentData.minLevel, currentData.maxLevel, DEFAULT_LEVEL_VALUE);

            Utility::GetIniValue(ini, chanceString, section, "Chance",
                              DEFAULT_CHANCE_STRING);
            //logger::info("Chance: {}", chanceString);

            currentData.chance = std::clamp(
                Utility::StringToFloat(chanceString, DEFAULT_CHANCE_VALUE),
                DEFAULT_CHANCE_MIN, DEFAULT_CHANCE_MAX);

            Utility::GetIniValue(ini, useAllString, section, "UseAll",
                              DEFAULT_USE_ALL_STRING);
            //logger::info("UseAll: {}", useAllString);

            currentData.useAll = Utility::StringToUnInt(useAllString, DEFAULT_USE_ALL_VALUE);

            DirectProtocol(currentData);

            //logger::info("READ COMPLETE");
        }
    }

    return totalDataSize >= 1;
}


template<typename T> bool Manager::ProcessBatchLeveledList(const RE::FormType& formType, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& map, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& keywordMap)
{
    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    const auto& lists = dataHandler->GetFormArray(formType);

    const RE::BSTArrayBase::size_type numberOfLists = lists.size();

    SmallerLeveledObject originalBuffer[Data::MAX_ENTRY_SIZE];
    SmallerLeveledObject insertBuffer[Data::MAX_ENTRY_SIZE];

    // not to be confused with capacity
    size_t originalBufferElements;
    size_t insertBufferElements;

    std::vector<ItemData*> resetVector;

    size_t oldListSize;
    size_t newListSize;
    size_t currentItemsSize;
    size_t resetVectorSize;
    size_t currentKeywordItemsSize;

    std::uniform_real_distribution<float> randomDistributor(0.0f, 99.99f);

    bool ongoing;
    bool resizePending;
    bool keepOriginal;
    bool listDoesNotUseAll;
    bool keywordSearching = !keywordMap.empty();

    resetVector.reserve(Data::MAX_ENTRY_SIZE);

    // check list entries for batch inserts
    for (RE::BSTArrayBase::size_type i = 0; i < numberOfLists; ++i)
    {
        T *currentList = lists[i] ? lists[i]->As<T>() : nullptr;

        if (currentList)
        {
            originalBufferElements = 0;
            insertBufferElements = 0;
            ongoing = true;
            resizePending = false;
            listDoesNotUseAll = ((currentList->llFlags & RE::TESLeveledList::Flag::kUseAll) == 0);

            RE::SimpleArray<RE::LEVELED_OBJECT>& currentEntries = currentList->entries;
            oldListSize = std::min((size_t)currentList->numEntries, currentEntries.size()); // guaranteed to be Data::MAX_ENTRY_SIZE (255) or lower because currentList->numEntries is an unsigned byte with max value 0xFF (255)

            for (RE::SimpleArray<RE::LEVELED_OBJECT>::size_type j = 0; (j < oldListSize); ++j)
            {
                keepOriginal = true;
                auto currentForm = currentEntries[j].form;

                if (currentForm && ongoing)
                {
                    if (auto mapIterator = map.find(currentForm->formID); mapIterator != map.end())
                    {
                        std::vector<ItemData>& currentItems = mapIterator->second;

                        currentItemsSize = std::min(currentItems.size(), Data::MAX_ENTRY_SIZE);

                        for (size_t k = 0; (k < currentItemsSize) && ongoing; ++k)
                        {
                            if (currentItems[k].processCounter > 0)
                            {
                                if (listDoesNotUseAll || currentItems[k].useAll)
                                {
                                    if (randomDistributor(randomEngine) < currentItems[k].chance)
                                    {
                                        if (ProcessBatchProtocol(currentItems[k], currentEntries[j], insertBufferElements, insertBuffer, keepOriginal, resetVector))
                                        {
                                            resizePending = true;
                                        }

                                        if (insertBufferElements >= Data::MAX_ENTRY_SIZE)
                                        {
                                            // won't be able to insert more items into list
                                            ongoing = false;
                                        }
                                    }
                                    else
                                    {
                                        ++totalListChanceSkips;
                                    }
                                }
                                else
                                {
                                    ++totalListUseAllSkips;
                                }
                            }
                        }
                    }

                    if (auto keywordForm = currentForm->As<RE::BGSKeywordForm>(); keywordForm && keywordSearching)
                    {
                        for (uint32_t x = 0; (x < keywordForm->numKeywords) && ongoing; ++x)
                        {
                            if (auto currentKeyword = keywordForm->GetKeywordAt(x); currentKeyword.has_value())
                            {
                                if (auto keywordMapIterator = keywordMap.find(currentKeyword.value()->formID); keywordMapIterator != keywordMap.end())
                                {
                                    std::vector<ItemData>& currentKeywordItems = keywordMapIterator->second;

                                    currentKeywordItemsSize = std::min(Data::MAX_ENTRY_SIZE - insertBufferElements, currentKeywordItems.size());

                                    for (size_t y = 0; (y < currentKeywordItemsSize) && ongoing; ++y)
                                    {
                                        if (currentKeywordItems[y].processCounter > 0)
                                        {
                                            if (listDoesNotUseAll || currentKeywordItems[y].useAll)
                                            {
                                                if (randomDistributor(randomEngine) < currentKeywordItems[y].chance)
                                                {
                                                    if (ProcessBatchProtocol(currentKeywordItems[y], currentEntries[j], insertBufferElements, insertBuffer, keepOriginal, resetVector))
                                                    {
                                                        resizePending = true;
                                                    }

                                                    if (insertBufferElements >= Data::MAX_ENTRY_SIZE)
                                                    {
                                                        // won't be able to insert more items into list
                                                        ongoing = false;
                                                    }
                                                }
                                                else
                                                {
                                                    ++totalListChanceSkips;
                                                }
                                            }
                                            else
                                            {
                                                ++totalListUseAllSkips;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (keepOriginal)
                {
                    // copy original object data into originalBuffer[originalBufferElements]
                    originalBuffer[originalBufferElements].form = currentEntries[j].form;
                    originalBuffer[originalBufferElements].count = currentEntries[j].count;
                    originalBuffer[originalBufferElements].level = currentEntries[j].level;

                    ++originalBufferElements;
                }
            }

            if (resizePending)
            {
                // newListSize guaranteed 255 or less
                newListSize = std::min(originalBufferElements + insertBufferElements, Data::MAX_ENTRY_SIZE);

                if (newListSize == 0)
                {
                    // currentEntries.resize(0)
                    currentEntries.clear();
                }
                else
                {
                    currentEntries.resize(newListSize);

                    // calculate maximum number of inserts allowed beforehand
                    insertBufferElements = std::min(insertBufferElements, Data::MAX_ENTRY_SIZE - originalBufferElements);

                    // insert data from originalBuffer and insertBuffer to leveled list entries
                    InsertLeveledListBuffers(insertBufferElements, insertBuffer, originalBufferElements, originalBuffer, currentEntries);

                    // need to sort by level and null extra data pointers
                    //Utility::SortLeveledListArrayByLevel(currentEntries);
                    std::sort(currentEntries.begin(), currentEntries.end(), Utility::CompareLeveledListEntryLevel);
                    Utility::SanitizeLeveledListArray(currentEntries);
                }

                currentList->numEntries = newListSize;

                resetVectorSize = resetVector.size();

                if (resetVectorSize > 0)
                {
                    // reset process counters for next loop
                    for (size_t m = 0; m < resetVectorSize; ++m)
                    {
                        resetVector[m]->processCounter = 1;
                    }

                    resetVector.clear();
                    resetVector.reserve(Data::MAX_ENTRY_SIZE);
                }

                ++uniqueListBatchModified;
                totalListInserts += insertBufferElements;
                totalListRemovals += oldListSize - originalBufferElements;
            }
        }
    }

    return false;
}

// necessary signatures for the linker
template bool Manager::ProcessBatchLeveledList<RE::TESLevItem>(const RE::FormType& formType, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& map, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& keywordMap);
template bool Manager::ProcessBatchLeveledList<RE::TESLevCharacter>(const RE::FormType& formType, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& map, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& keywordMap);
template bool Manager::ProcessBatchLeveledList<RE::TESLevSpell>(const RE::FormType& formType, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& map, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& keywordMap);

template<typename T> bool Manager::ProcessFocusLeveledList(const RE::FormType& formType, boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>>& map)
{
    SmallerLeveledObject originalBuffer[Data::MAX_ENTRY_SIZE];

    // not to be confused with capacity
    size_t originalBufferElements;

    size_t oldListSize;
    size_t newListSize;
    size_t currentItemsSize;
    size_t insertLimit;

    std::uniform_real_distribution<float> randomDistributor(0.0f, 99.99f);

    bool insertOngoing;
    bool resizePending;
    bool keepOriginal;
    bool listDoesNotUseAll;

    // check list for focus inserts
    for (auto& [targetKey, pairValue] : map)
    {
        T* targetForm = RE::TESForm::LookupByID<T>(targetKey);

        if (targetForm)
        {
            originalBufferElements = 0;
            insertOngoing = true;
            
            resizePending = false;

            RE::SimpleArray<RE::LEVELED_OBJECT>& currentEntries = targetForm->entries;
            std::vector<ItemData>& insertDataList = pairValue.first;
            boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>& removeDataList = pairValue.second;

            oldListSize = std::min((size_t)(targetForm->numEntries), currentEntries.size()); // guaranteed to be Data::MAX_ENTRY_SIZE (255) or lower because currentList->numEntries is an unsigned byte with max value 0xFF (255)

            listDoesNotUseAll = ((targetForm->llFlags & RE::TESLeveledList::Flag::kUseAll) == 0);

            // preparing removals
            for (size_t i = 0; i < oldListSize; ++i)
            {
                keepOriginal = true;
                auto currentForm = currentEntries[i].form;

                if (currentForm)
                {
                    if (auto mapIterator = removeDataList.find(currentForm->formID); mapIterator != removeDataList.end())
                    {
                        std::vector<SmallerItemData>& removeDataVector = mapIterator->second;

                        currentItemsSize = removeDataVector.size();

                        for (size_t k = 0; (k < currentItemsSize) && keepOriginal; ++k)
                        {
                            if (removeDataVector[k].processCounter > 0)
                            {
                                if (listDoesNotUseAll || removeDataVector[k].useAll)
                                {
                                    if (randomDistributor(randomEngine) < removeDataVector[k].chance)
                                    {
                                        if (ProcessFocusProtocolRemove(removeDataVector[k], keepOriginal))
                                        {
                                            resizePending = true;
                                        }
                                    }
                                    else
                                    {
                                        ++totalListChanceSkips;
                                    }
                                }
                                else
                                {
                                    ++totalListUseAllSkips;
                                }
                            }
                        }
                    }
                }

                if (keepOriginal)
                {
                    originalBuffer[originalBufferElements].form = currentEntries[i].form;
                    originalBuffer[originalBufferElements].count = currentEntries[i].count;
                    originalBuffer[originalBufferElements].level = currentEntries[i].level;
                    ++originalBufferElements;
                }
            }

            // removals prepped

            insertLimit = std::min(insertDataList.size(), Data::MAX_ENTRY_SIZE - originalBufferElements);

            if (resizePending || insertLimit > 0)
            {
                newListSize = std::min(originalBufferElements + insertLimit, Data::MAX_ENTRY_SIZE);

                if (newListSize == 0)
                {
                    // currentEntries.resize(0)
                    currentEntries.clear();
                } 
                else
                {
                    currentEntries.resize(newListSize);

                    // insert data from originalBuffer and insertBuffer to leveled list entries
                    InsertLeveledListVectorBuffer(insertLimit, insertDataList, originalBufferElements, originalBuffer, currentEntries);

                    // need to sort by level and null extra data pointers
                    //Utility::SortLeveledListArrayByLevel(currentEntries);
                    std::sort(currentEntries.begin(), currentEntries.end(), Utility::CompareLeveledListEntryLevel);
                    Utility::SanitizeLeveledListArray(currentEntries);
                }

                targetForm->numEntries = newListSize;

                ++uniqueListBatchModified;
                totalListInserts += insertLimit;
                totalListRemovals += oldListSize - originalBufferElements;
            }
        }
    }

    return false;
}

// necessary signatures for the linker
template bool Manager::ProcessFocusLeveledList<RE::TESLevItem>(const RE::FormType& formType, boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>>& map);
template bool Manager::ProcessFocusLeveledList<RE::TESLevCharacter>(const RE::FormType& formType, boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>>& map);
template bool Manager::ProcessFocusLeveledList<RE::TESLevSpell>(const RE::FormType& formType, boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>>& map);

bool Manager::DirectProtocol(ItemData& data)
{
    const auto protocol = data.protocol;

    //if (Utility::CheckCompatibleOutfitFormTypes(data.insertFormType, data.targetFormType))
    if(Utility::CheckCompatibleKeywordFormTypes(data.insertFormType, data.targetFormType))
    {
        if ((protocol >= Data::VALID_SINGLE_PROTOCOL_INSERT_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_INSERT_MAX))
        {
            data.processCounter = 1;
            return InsertIntoKeywordMap(data);
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_INSERT_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_INSERT_MAX))
        {
            data.processCounter = Data::MAX_ENTRY_SIZE;
            return InsertIntoKeywordMap(data);
        }
        else if ((protocol >= Data::VALID_SINGLE_PROTOCOL_REMOVE_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_REMOVE_MAX))
        {
            data.processCounter = 1;
            return InsertIntoKeywordMap(data);
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_REMOVE_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_REMOVE_MAX))
        {
            data.processCounter = Data::MAX_ENTRY_SIZE;
            return InsertIntoKeywordMap(data);
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_SWAP_GENERATE_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_SWAP_GENERATE_MAX))
        {
            data.processCounter = 1;
            return insertGeneratedKeywordMap(data);
        }
    }
    else if (Utility::CheckCompatibleLeveledListFormTypes(data.insertFormType, data.targetFormType)) // check if form types are compatible
    {
        if ((protocol >= Data::VALID_SINGLE_PROTOCOL_INSERT_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_INSERT_MAX))
        {
            data.processCounter = 1;
            return InsertIntoBatchMap(data);
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_INSERT_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_INSERT_MAX))
        {
            data.processCounter = Data::MAX_ENTRY_SIZE;
            return InsertIntoBatchMap(data);
        }
        else if ((protocol >= Data::VALID_SINGLE_PROTOCOL_REMOVE_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_REMOVE_MAX))
        {
            data.processCounter = 1;
            return InsertIntoBatchMap(data);
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_REMOVE_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_REMOVE_MAX))
        {
            data.processCounter = Data::MAX_ENTRY_SIZE;
            return InsertIntoBatchMap(data);
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_SWAP_GENERATE_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_SWAP_GENERATE_MAX))
        {
            data.processCounter = 1;
            return InsertGeneratedBatchMap(data);
        }
        else if ((protocol >= Data::VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_MAX))
        {
            data.processCounter = 1;
            return InsertIntoFocusMapAdd(data);
        }
        else if ((protocol >= Data::VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MAX))
        {
            data.processCounter = 1;
            return InsertIntoFocusMapRemove(data);
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MAX))
        {
            data.processCounter = Data::MAX_ENTRY_SIZE;
            return InsertIntoFocusMapRemove(data);
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_OUTFIT_SWAP_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_OUTFIT_SWAP_MIN))
        {
            //data.processCounter = Data::MAX_ENTRY_SIZE;
            return InsertIntoOutfitMap(data);
        }/*
        else if((protocol >= Data::VALID_SINGLE_PROTOCOL_CONTAINER_INSERT_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_CONTAINER_INSERT_MAX))
        {
            data.processCounter = 1;
            return InsertIntoContainerDirectMap(data);
        }
        else if((protocol >= Data::VALID_MULTI_PROTOCOL_CONTAINER_INSERT_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_CONTAINER_INSERT_MAX))
        {
            data.processCounter = Data::MAX_ENTRY_SIZE;
            return InsertIntoContainerDirectMap(data);
        }
        else if((protocol >= Data::VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_MAX))
        {
            data.processCounter = 1;
            return InsertIntoContainerDirectMap(data);
        }
        else if((protocol >= Data::VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_MAX))
        {
            data.processCounter = Data::MAX_ENTRY_SIZE;
            return InsertIntoContainerDirectMap(data);
        }*//*
        else if((protocol >= Data::VALID_SINGLE_PROTOCOL_CONTAINER_SWAP_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_CONTAINER_SWAP_MAX))
        {
            return InsertIntoContainerGenerateMap(data);
        }*/
        else if((protocol >= Data::VALID_MULTI_PROTOCOL_CONTAINER_SWAP_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_CONTAINER_SWAP_MAX))
        {
            return InsertIntoContainerGenerateMapLite(data);
        }

    } // maybe allow keyword formtype through to remove any items with those keywords from all leveled lists?
    else if((data.targetFormType >= Data::ITEM_FORM_TYPE) && (data.targetFormType <= Data::LEVELED_SPELL_FORM_TYPE)) // form types are not compatible, exclude keyword formtype
    {
        // check to see if protocol doesn't require valid insert
        // add to appropriate map
        switch (protocol)
	    {
		case Data::VALID_SINGLE_PROTOCOL_NO_INSERT_REMOVE:

            data.processCounter = 1;
            return InsertIntoBatchMap(data);
        case Data::VALID_MULTI_PROTOCOL_NO_INSERT_REMOVE:

            data.processCounter = Data::MAX_ENTRY_SIZE;
            return InsertIntoBatchMap(data);
	    }
    }

    ++removedDataCounter;
    return false;
}

bool Manager::InsertIntoBatchMap(ItemData& data)
{
    switch (data.targetFormType)
	{
    case Data::ITEM_FORM_TYPE:
    case Data::ARMOR_FORM_TYPE:
    case Data::LEVELED_ITEM_FORM_TYPE:
        
        return InsertIntoCommonMap(data, itemMap);
    case Data::NPC_FORM_TYPE:
    case Data::LEVELED_NPC_FORM_TYPE:
        
        return InsertIntoCommonMap(data, npcMap);
    case Data::SPELL_FORM_TYPE:
    case Data::LEVELED_SPELL_FORM_TYPE:
        
        return InsertIntoCommonMap(data, spellMap);
	}

    ++removedDataCounter;
    return false;
}

bool Manager::InsertIntoFocusMapAdd(ItemData& data)
{
    switch (data.targetFormType)
	{
    case Data::LEVELED_ITEM_FORM_TYPE:
        
        return InsertIntoWeirdMapAdd(data, itemLeveledHybridMap);
    case Data::LEVELED_NPC_FORM_TYPE:
        
        return InsertIntoWeirdMapAdd(data, npcLeveledHybridMap);
    case Data::LEVELED_SPELL_FORM_TYPE:
        
        return InsertIntoWeirdMapAdd(data, spellLeveledHybridMap);
	}

    ++removedDataCounter;
    return false;
}

bool Manager::InsertIntoFocusMapRemove(ItemData& data)
{
    switch (data.targetFormType)
	{
    case Data::LEVELED_ITEM_FORM_TYPE:
        
        return InsertIntoWeirdMapRemove(data, itemLeveledHybridMap);
    case Data::LEVELED_NPC_FORM_TYPE:
        
        return InsertIntoWeirdMapRemove(data, npcLeveledHybridMap);
    case Data::LEVELED_SPELL_FORM_TYPE:
        
        return InsertIntoWeirdMapRemove(data, spellLeveledHybridMap);
	}

    ++removedDataCounter;
    return false;
}

bool Manager::InsertIntoCommonMap(ItemData& data, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& map)
{
    if (auto mapIterator = map.find(data.targetForm->formID); mapIterator != map.end())
    {
        mapIterator->second.emplace_back(data);

        ++totalDataSize;

        return true;
    }
    else
    {
        map.emplace(data.targetForm->formID, std::vector<ItemData>());
        map.at(data.targetForm->formID).emplace_back(data);

        ++totalTargetSize;
        ++totalDataSize;

        return true;
    }

    ++removedDataCounter;
    return false;
}

// hear me out
bool Manager::InsertIntoWeirdMapAdd(ItemData& data, boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>>& map)
{
    if (auto mapIterator = map.find(data.targetForm->formID); mapIterator != map.end())
    {
        mapIterator->second.first.emplace_back(data);

        ++totalDataSize;

        return true;
    }
    else
    {
        map.emplace(data.targetForm->formID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>());
        map.at(data.targetForm->formID).first.emplace_back(data);

        ++totalTargetSize;
        ++totalDataSize;

        return true;
    }

    ++removedDataCounter;
    return false;
}

// hear me out tho
bool Manager::InsertIntoWeirdMapRemove(ItemData& data, boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>>& map)
{
    if (auto mapIterator = map.find(data.targetForm->formID); mapIterator != map.end())
    {
            
        if (auto innerIterator = mapIterator->second.second.find(data.insertForm->formID); innerIterator != mapIterator->second.second.end())
        {
            //innerIterator->second.emplace_back(SmallerItemData(data));
            innerIterator->second.emplace_back(data);
            ++totalDataSize;

            return true;
        }
        else
        {
            mapIterator->second.second.emplace(data.insertForm->formID, std::vector<SmallerItemData>());
            //mapIterator->second.second.at(data.insertForm->formID).emplace_back(SmallerItemData(data));
            mapIterator->second.second.at(data.insertForm->formID).emplace_back(data);
            ++totalDataSize;

            return true;
        }
    }
    else
    {
        map.emplace(data.targetForm->formID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>());

        // declared in if statement
        mapIterator = map.find(data.targetForm->formID);

        if (auto innerIterator = mapIterator->second.second.find(data.insertForm->formID); innerIterator != mapIterator->second.second.end())
        {
            //innerIterator->second.emplace_back(SmallerItemData(data));
            innerIterator->second.emplace_back(data);
            ++totalTargetSize;
            ++totalDataSize;

            return true;
        }
        else
        {
            mapIterator->second.second.emplace(data.insertForm->formID, std::vector<SmallerItemData>());
            //mapIterator->second.second.at(data.insertForm->formID).emplace_back(SmallerItemData(data));
            mapIterator->second.second.at(data.insertForm->formID).emplace_back(data);
            ++totalTargetSize;
            ++totalDataSize;

            return true;
        }

        return true;
    }

    ++removedDataCounter;
    return false;
}

bool Manager::InsertIntoKeywordMap(ItemData& data)
{
    // Might need valid insert field present for removals only if implemented to differentiate from item lists, npc lists, and spell lists
    // could create a universal key map, but they would require more hash lookups if not more loops
    switch (data.insertFormType) // has to use insertFormType since targetFormType should be keyword.
	{
    case Data::ITEM_FORM_TYPE:
    case Data::ARMOR_FORM_TYPE:
    case Data::LEVELED_ITEM_FORM_TYPE:
        
        return InsertIntoCommonMap(data, itemKeywordMap);
    case Data::NPC_FORM_TYPE:
    case Data::LEVELED_NPC_FORM_TYPE:
        
        return InsertIntoCommonMap(data, npcKeywordMap);
    case Data::SPELL_FORM_TYPE:
    case Data::LEVELED_SPELL_FORM_TYPE:
        
        return InsertIntoCommonMap(data, spellKeywordMap);
	}

    ++removedDataCounter;
    return false;
}

/// <summary>
/// checks data's protocol and processes it into a LEVELED_OBJECT for insertion into insertBuffer
/// and/or keeps originalObject by moving its data into originalBuffer
/// or allows originalObject to be removed when its leveled list is resized
/// note that arguments are passed by reference to be modified
/// </summary>
/// <param name="data"> The data to insert or determine if originalObject should be removed when the leveled list will resize </param>
/// <param name="originalObject"> the original object in the leveled list that matched into the map. Will be placed into originalBufferElements if the data's protocol allows it </param>
/// <param name="insertBufferElements"> number of elements in insertBuffer, not to be confused with max capacity (255) </param>
/// <param name="insertBuffer"> array of LEVELED_OBJECTS that will receive LEVELED_OBJECT data based on data if data's protocol is for insert </param>
/// <param name="keepOriginal"> will be set to false if a protocol wants the original object removed, otherwise retains passed value </param>
/// <returns> true if an insertion or removal occurred, otherwise false </returns>
bool Manager::ProcessBatchProtocol(ItemData& data, RE::LEVELED_OBJECT& originalObject, std::size_t& insertBufferElements, SmallerLeveledObject* insertBuffer, bool& keepOriginal, std::vector<ItemData*>& resetVector)
{
    switch (data.protocol)
	{
        // ----- BATCH MAPS -----
        // 0-49
		case Data::VALID_SINGLE_PROTOCOL_INSERT_BASIC:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;

            data.processCounter = 0;
            resetVector.push_back(&data);

            ++insertBufferElements;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_INSERT_BASIC_COUNT:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;

            data.processCounter = 0;
            resetVector.push_back(&data);
            
            ++insertBufferElements;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_INSERT_BASIC_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = originalObject.level;

            data.processCounter = 0;
            resetVector.push_back(&data);

            ++insertBufferElements;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_INSERT_BASIC_COUNT_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = originalObject.level;

            data.processCounter = 0;
            resetVector.push_back(&data);

            ++insertBufferElements;
            return true;
        // 50-99
        case Data::VALID_MULTI_PROTOCOL_INSERT_BASIC:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;

            ++insertBufferElements;
            return true;
        case Data::VALID_MULTI_PROTOCOL_INSERT_BASIC_COUNT:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;

            ++insertBufferElements;
            return true;
        case Data::VALID_MULTI_PROTOCOL_INSERT_BASIC_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = originalObject.level;

            ++insertBufferElements;
            return true;
        case Data::VALID_MULTI_PROTOCOL_INSERT_BASIC_COUNT_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = originalObject.level;

            ++insertBufferElements;
            return true;
        // 100-149
        case Data::VALID_SINGLE_PROTOCOL_REMOVE_BASIC:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;

            data.processCounter = 0;
            resetVector.push_back(&data);

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_REMOVE_BASIC_COUNT:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;

            data.processCounter = 0;
            resetVector.push_back(&data);

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_REMOVE_BASIC_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = originalObject.level;

            data.processCounter = 0;
            resetVector.push_back(&data);

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_REMOVE_BASIC_COUNT_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = originalObject.level;

            data.processCounter = 0;
            resetVector.push_back(&data);

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_NO_INSERT_REMOVE:

            data.processCounter = 0;
            resetVector.push_back(&data);

            keepOriginal = false;
            return true;
        // 150-199
        case Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC_COUNT:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;
            
            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = originalObject.level;
            
            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC_COUNT_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].form = data.insertForm;
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = originalObject.level;

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_MULTI_PROTOCOL_NO_INSERT_REMOVE:
            keepOriginal = false;
            return true;
	}

    ++wrongDataCounter;
    return false;
}

// unnecessary
bool Manager::ProcessFocusProtocolAdd(ItemData& data, RE::LEVELED_OBJECT& originalObject, std::size_t& insertBufferElements, SmallerLeveledObject* insertBuffer)
{

    switch (data.protocol)
    {
    // ----- FOCUS MAPS -----
    // 200-249
    case Data::VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_BASIC:

        insertBuffer[insertBufferElements].form = data.insertForm;
        insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
        insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;

        data.processCounter = 0;

        ++insertBufferElements;

        return true;
    // 250-299
    }

    ++wrongDataCounter;
    return false;
}

bool Manager::ProcessFocusProtocolRemove(SmallerItemData& data, bool& keepOriginal)
{
    switch (data.protocol)
	{
    // ----- FOCUS MAPS -----
    // 300-349
    case Data::VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_BASIC:

        data.processCounter = 0;

        keepOriginal = false;
        return true;

    // 350-399
    case Data::VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_BASIC:

        keepOriginal = false;
        return true;
	}

    ++wrongDataCounter;
    return false;
}

void Manager::InsertLeveledListBuffers(const std::size_t insertBufferElements, SmallerLeveledObject* insertBuffer, const std::size_t originalBufferElements, SmallerLeveledObject* originalBuffer, RE::SimpleArray<RE::LEVELED_OBJECT>& entries)
{
    for (size_t i = 0; i < originalBufferElements; ++i) // shouldn't need entriesCapacity check
    {
        entries[i].form = originalBuffer[i].form;
        entries[i].count = originalBuffer[i].count;
        entries[i].level = originalBuffer[i].level;
    }

    //for (size_t j = 0; (j < insertBufferElements) && ((j + originalBufferElements) < entriesCapacity); ++j)
    for (size_t j = 0; j < insertBufferElements; ++j) // calculate insert limit before
    {
        entries[(j + originalBufferElements)].form = insertBuffer[j].form;
        entries[(j + originalBufferElements)].count = insertBuffer[j].count;
        entries[(j + originalBufferElements)].level = insertBuffer[j].level;
    }
}


void Manager::InsertLeveledListVectorBuffer(const std::size_t insertLimit, std::vector<ItemData>& insertDataVector, const std::size_t originalBufferElements, SmallerLeveledObject* originalBuffer, RE::SimpleArray<RE::LEVELED_OBJECT>& entries)
{
    for (size_t i = 0; i < originalBufferElements; ++i)
    {
        entries[i].form = originalBuffer[i].form;
        entries[i].count = originalBuffer[i].count;
        entries[i].level = originalBuffer[i].level;
    }

    for (size_t j = 0; j < insertLimit; ++j)
    {
        entries[(j + originalBufferElements)].form = insertDataVector[j].insertForm;
        entries[(j + originalBufferElements)].count = (rand() % (insertDataVector[j].maxCount - insertDataVector[j].minCount + 1)) + insertDataVector[j].minCount;
        entries[(j + originalBufferElements)].level = (rand() % (insertDataVector[j].maxLevel - insertDataVector[j].minLevel + 1)) + insertDataVector[j].minLevel;
    }
}


bool Manager::InsertIntoOutfitMap(ItemData& data)
{
    /*
    if (Utility::CheckCompatibleOutfitFormTypes(data.insertFormType, data.targetFormType))
    {
        if (auto mapIterator = itemOutfitMap.find(data.targetForm->formID); mapIterator != itemOutfitMap.end())
        {
            mapIterator->second.second.emplace_back(data);

            ++totalDataSize;

            return true;
        }
        else
        {
            itemOutfitMap.emplace(data.targetForm->formID, std::pair<RE::TESLevItem*, std::vector<OutfitItemData>>());
            itemOutfitMap.at(data.targetForm->formID).second.emplace_back(data);

            ++totalTargetSize;
            ++totalDataSize;

            return true;
        }
    }

    ++wrongDataCounter;
    return false;
    */

    if (Utility::CheckCompatibleOutfitFormTypes(data.insertFormType, data.targetFormType))
    {
        // single hash lookup
        auto [iterator, wasInserted] = itemOutfitMap.emplace(
            std::piecewise_construct, 
            std::forward_as_tuple(data.targetForm->formID), 
            std::forward_as_tuple()
        );

        iterator->second.insertVector.emplace_back(data);
        iterator->second.targetForm = data.targetForm;
        iterator->second.targetFormType = data.targetFormType;

        Utility::SetGeneratedLeveledListInstruction(iterator->second.instruction, data.useAll);

        return true;
    }

    ++wrongDataCounter;
    return false;
}

bool Manager::ProcessBatchOutfit()
{
    auto& outfitList = RE::TESDataHandler::GetSingleton()->GetFormArray<RE::BGSOutfit>();
    const size_t outfitListSize = outfitList.size();
    size_t outfitItemsSize;
    bool outfitModified;

    for (size_t i = 0; i < outfitListSize; ++i)
    {
        if (outfitList[i])
        {
            outfitModified = false;
            auto& outfitItems = outfitList[i]->outfitItems;
            outfitItemsSize = outfitItems.size();

            for (size_t k = 0; k < outfitItemsSize; ++k)
            {
                if (outfitItems[k])
                {
                    if (auto mapIterator = itemOutfitMap.find(outfitItems[k]->formID); mapIterator != itemOutfitMap.end())
                    {
                        if (mapIterator->second.generatedLeveledList)
                        {
                            outfitItems[k] = mapIterator->second.generatedLeveledList;
                            ++totalOutfitSwaps;
                            outfitModified = true;
                        }
                    }
                }
            }

            if (outfitModified)
            {
                ++uniqueOutfitBatchModified;
            }
        }
    }

    return false;
}

bool Manager::GenerateOutfitLeveledLists()
{
    #if defined(USING_UPKEEP_MANAGER)
    const auto upkeepManager = UpkeepManager::GetSingleton();
    #endif
    bool generated = false;
    
    for (auto& [targetKey, pairValue] : itemOutfitMap)
    {
        //if (!pairValue.first)
        //{
        pairValue.generatedLeveledList = Utility::GenerateLeveledList<RE::TESLevItem*>(pairValue, totalGeneratedLeveledListTargetReinserts);

        if (pairValue.generatedLeveledList)
        {
            ++totalLeveledListGenerated;
            #if defined(USING_UPKEEP_MANAGER)
            upkeepManager->InsertLookupBatch(pairValue.first);
            #endif
            generated = true;
        }
        //}
    }

    return generated;
}

/*
bool Manager::InsertIntoContainerDirectMap(ItemData& data)
{
    if (Utility::CheckCompatibleContainerFormTypes(data.insertFormType, data.targetFormType))
    {
        if (auto mapIterator = itemContainerMap.find(data.targetForm->formID); mapIterator != itemContainerMap.end())
        {
            mapIterator->second.second.emplace_back(data);

            ++totalDataSize;

            return true;
        }
        else
        {
            itemContainerMap.emplace(data.targetForm->formID, std::pair<ContainerGenerateData, std::vector<ContainerDirectItemData>>());

            // declared in first if statement
            mapIterator = itemContainerMap.find(data.targetForm->formID);

            mapIterator->second.second.emplace_back(data);

            ++totalTargetSize;
            ++totalDataSize;

            return true;
        }
    }

    ++wrongDataCounter;
    return false;
}
*/

/*
bool Manager::InsertIntoContainerGenerateMap(ItemData& data)
{
    if (Utility::CheckCompatibleContainerFormTypes(data.insertFormType, data.targetFormType))
    {
        if (auto mapIterator = itemContainerMap.find(data.targetForm->formID); mapIterator != itemContainerMap.end())
        {
            mapIterator->second.first.vector.emplace_back(data);

            ++totalDataSize;

            return true;
        }
        else
        {
            itemContainerMap.emplace(data.targetForm->formID, std::pair<ContainerGenerateData, std::vector<ContainerDirectItemData>>());

            // declared in first if statement
            mapIterator = itemContainerMap.find(data.targetForm->formID);

            mapIterator->second.first.vector.emplace_back(data);

            ++totalTargetSize;
            ++totalDataSize;

            return true;
        }
    }

    ++wrongDataCounter;
    return false;
}
*/

/*
bool Manager::GenerateContainerLeveledLists()
{
    //bool generated = false;
    
    for (auto& [targetKey, pairValue] : itemContainerMap)
    {
        //if (!pairValue.first.leveledList)
        //{
        pairValue.first.leveledList = Utility::CreateLeveledList(pairValue.first.vector);
        
        //if (pairValue.first.leveledList)
        //{
        //    generated = true;
        //}
        //}
    }

    return true;
}
*/

/*
bool Manager::ProcessBatchContainer()
{
    auto& containerList = RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESObjectCONT>();
    size_t containerListSize = containerList.size();

    SmallerContainerObject originalBuffer[Data::MAX_CONTAINER_MODIFY_SIZE];
    std::vector<SmallerContainerObject> insertBuffer; // resizable buffer
    std::vector<ContainerDirectItemData*> resetVector;
    //size_t insertBufferCapacity = Data::MAX_CONTAINER_MODIFY_SIZE;
    //size_t insertBufferElements;
    size_t originalBufferElements;
    size_t directItemVectorSize;
    size_t resetVectorSize;

    std::uniform_real_distribution<float> randomDistributor(0.0f, 99.99f);

    uint32_t numContainerObjects;

    bool resizePending;
    bool keepOriginal;

    insertBuffer.reserve(Data::MAX_CONTAINER_MODIFY_SIZE); // bypass early resizes
    resetVector.reserve(Data::MAX_CONTAINER_MODIFY_SIZE);

    for (size_t i = 0; i < containerListSize; ++i)
    {
        if (containerList[i] && ((numContainerObjects = containerList[i]->numContainerObjects) < Data::MAX_CONTAINER_MODIFY_SIZE)) // single access with assignment in conditional is cruise control for cool
        {
            originalBufferElements = 0;
            //insertBufferElements = 0;
            resizePending = false;
            keepOriginal = true;

            for (size_t k = 0; k < numContainerObjects; ++k) // will also bypass containers with 0 items, surprisingly a lot of them
            {
                auto containerObject = (containerList[i]->containerObjects ? containerList[i]->containerObjects[k] : nullptr);

                if (containerObject && containerObject->obj)
                {
                    if (auto mapIterator = itemContainerMap.find(containerObject->obj->formID); mapIterator != itemContainerMap.end())
                    {
                        auto& directItemVector = mapIterator->second.second;
                        directItemVectorSize = directItemVector.size();

                        for (size_t x = 0; x < directItemVectorSize; ++x)
                        {
                            if (directItemVector[x].processCounter > 0)
                            {
                                if (randomDistributor(randomEngine) < directItemVector[x].chance)
                                {
                                    
                                    if (ProcessContainerBatchProtocol(directItemVector[x], containerObject, insertBuffer, keepOriginal, resetVector))
                                    {
                                        resizePending = true;
                                    }
                                }
                                else
                                {
                                    ++totalContainerChanceSkips;
                                }
                            }
                        }

                        if (mapIterator->second.first.leveledList)
                        {
                            // basically protocol handling for this case
                            if (keepOriginal)
                            {
                                // direct swap
                                containerObject->obj = mapIterator->second.first.leveledList;
                                ++totalContainerSwaps;
                            }
                            else
                            {
                                insertBuffer.emplace_back(containerObject->count, mapIterator->second.first.leveledList);
                                resizePending = true;
                                ++totalContainerSwaps;
                            }
                        }
                    }

                    if (keepOriginal)
                    {
                        insertBuffer.emplace_back(containerObject->count, containerObject->obj);
                    }
                }
            }

            if (resizePending)
            {
                ClearAndInsertContainer(containerList[i], insertBuffer);

                ++uniqueContainersBatchModified;

                if (insertBuffer.size() > 0)
                {
                    insertBuffer.clear();
                    insertBuffer.reserve(Data::MAX_CONTAINER_MODIFY_SIZE);
                }

                resetVectorSize = resetVector.size();

                if (resetVectorSize > 0)
                {
                    // reset process counters for next loop
                    for (size_t m = 0; m < resetVectorSize; ++m)
                    {
                        resetVector[m]->processCounter = 1;
                    }

                    resetVector.clear();
                    resetVector.reserve(Data::MAX_CONTAINER_MODIFY_SIZE);
                }
            }
        }
    }

    return false;
}
*/

bool Manager::ProcessBatchContainerLite()
{
    auto& containerList = RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESObjectCONT>();
    size_t containerListSize = containerList.size();

    uint32_t numContainerObjects;

    bool containerModified;

    for (size_t i = 0; i < containerListSize; ++i)
    {
        if (containerList[i] && ((numContainerObjects = containerList[i]->numContainerObjects) < Data::MAX_CONTAINER_MODIFY_SIZE)) // single access with assignment in conditional is cruise control for cool
        {
            containerModified = false;

            for (size_t k = 0; k < numContainerObjects; ++k) // will also bypass containers with 0 items, surprisingly a lot of them
            {
                auto containerObject = (containerList[i]->containerObjects ? containerList[i]->containerObjects[k] : nullptr);

                if (containerObject && containerObject->obj)
                {
                    if (auto mapIterator = itemContainerMapLite.find(containerObject->obj->formID); mapIterator != itemContainerMapLite.end())
                    {
                        if (mapIterator->second.generatedLeveledList)
                        {
                            // basically protocol handling for this case
                            // direct swap
                            containerObject->obj = mapIterator->second.generatedLeveledList;
                            ++totalContainerSwaps;
                            containerModified = true;
                        }
                    }
                }
            }

            if (containerModified)
            {
                ++uniqueContainersBatchModified;
            }
        }
    }

    return false;
}

/*
bool Manager::ProcessContainerBatchProtocol(ContainerDirectItemData& data, RE::ContainerObject* originalObject, std::vector<SmallerContainerObject>& insertBuffer, bool& keepOriginal, std::vector<ContainerDirectItemData*>& resetVector)
{
    switch (data.protocol)
	{
		case Data::VALID_SINGLE_PROTOCOL_CONTAINER_INSERT_BASIC:

            insertBuffer.emplace_back((rand() % (data.maxCount - data.minCount + 1)) + data.minCount, (RE::TESBoundObject*)data.insertForm);

            data.processCounter = 0;
            ++totalContainerInserts;

            return true;
        case Data::VALID_SINGLE_PROTOCOL_CONTAINER_INSERT_BASIC_COUNT:

            insertBuffer.emplace_back(originalObject->count, (RE::TESBoundObject*)data.insertForm);

            data.processCounter = 0;
            ++totalContainerInserts;

            return true;
        case Data::VALID_MULTI_PROTOCOL_CONTAINER_INSERT_BASIC:

            insertBuffer.emplace_back((rand() % (data.maxCount - data.minCount + 1)) + data.minCount, (RE::TESBoundObject*)data.insertForm);

            ++totalContainerInserts;

            return true;
        case Data::VALID_MULTI_PROTOCOL_CONTAINER_INSERT_BASIC_COUNT:

            insertBuffer.emplace_back(originalObject->count, (RE::TESBoundObject*)data.insertForm);

            ++totalContainerInserts;

            return true;
        case Data::VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_BASIC:

            insertBuffer.emplace_back((rand() % (data.maxCount - data.minCount + 1)) + data.minCount, (RE::TESBoundObject*)data.insertForm);

            data.processCounter = 0;
            ++totalContainerRemovals;
            keepOriginal = false;

            return true;
        case Data::VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_BASIC_COUNT:

            insertBuffer.emplace_back(originalObject->count, (RE::TESBoundObject*)data.insertForm);

            data.processCounter = 0;
            ++totalContainerRemovals;
            keepOriginal = false;

            return true;
        case Data::VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_NO_INSERT:

            data.processCounter = 0;
            ++totalContainerRemovals;
            keepOriginal = false;

            return true;
        case Data::VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_BASIC:

            insertBuffer.emplace_back((rand() % (data.maxCount - data.minCount + 1)) + data.minCount, (RE::TESBoundObject*)data.insertForm);

            ++totalContainerRemovals;
            keepOriginal = false;

            return true;
        case Data::VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_BASIC_COUNT:

            insertBuffer.emplace_back(originalObject->count, (RE::TESBoundObject*)data.insertForm);

            ++totalContainerRemovals;
            keepOriginal = false;

            return true;
        case Data::VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_NO_INSERT:

            ++totalContainerRemovals;
            keepOriginal = false;

            return true;
        
        // handled in ProcessBatchContainer function
        //case Data::VALID_MULTI_PROTOCOL_CONTAINER_SWAP_BASIC:
        //    return true;
        
	}

    ++wrongDataCounter;
    return false;
}
*/

void Manager::ClearAndInsertContainer(RE::TESContainer* container, std::vector<SmallerContainerObject>& insertBuffer)
{
    RE::free(container->containerObjects);

    const auto insertBufferSize = insertBuffer.size();

    if (insertBufferSize > 0)
    {
        container->containerObjects = RE::calloc<RE::ContainerObject*>(insertBufferSize);

        for (size_t i = 0; i < insertBufferSize; ++i)
        {
            container->containerObjects[i] = new RE::ContainerObject(insertBuffer[i].obj, insertBuffer[i].count);

            // maybe use ContainerObject instead of SmallerContainerObject to preserve owner?
            /*
            if(insertBuffer[i].ContainerItemExtra && insertBuffer[i].ContainerItemExtra->owner)
            {
                container->containerObjects[i] = new RE::ContainerObject(insertBuffer[i].obj, insertBuffer[i].count, insertBuffer[i].ContainerItemExtra->owner);
            }
            else
            {
                container->containerObjects[i] = new RE::ContainerObject(insertBuffer[i].obj, insertBuffer[i].count);
            }
            */
        }
    }
    /*
    else
    {
        container->containerObjects = nullptr; // insurance
    }
    */

    container->numContainerObjects = static_cast<uint32_t>(insertBufferSize);
}

bool Manager::InsertIntoContainerGenerateMapLite(ItemData& data)
{
    if (Utility::CheckCompatibleContainerFormTypes(data.insertFormType, data.targetFormType))
    {
        /*
        if (auto mapIterator = itemContainerMapLite.find(data.targetForm->formID); mapIterator != itemContainerMapLite.end())
        {
            mapIterator->second.insertVector.emplace_back(data);

            ++totalDataSize;

            return true;
        }
        else
        {
            itemContainerMapLite.emplace(data.targetForm->formID, ContainerGenerateData());

            // declared in first if statement
            mapIterator = itemContainerMapLite.find(data.targetForm->formID);

            mapIterator->second.insertVector.emplace_back(data);

            ++totalTargetSize;
            ++totalDataSize;

            return true;
        }
        */

        // single hash lookup
        auto [iterator, wasInserted] = itemContainerMapLite.emplace(
            std::piecewise_construct, 
            std::forward_as_tuple(data.targetForm->formID), 
            std::forward_as_tuple()
        );

        iterator->second.insertVector.emplace_back(data);

        Utility::SetGeneratedLeveledListInstruction(iterator->second.instruction, data.useAll);

        return true;
    }

    ++wrongDataCounter;
    return false;
}

bool Manager::GenerateContainerLeveledListsLite()
{
    for (auto& [targetKey, pairValue] : itemContainerMapLite)
    {
        pairValue.generatedLeveledList = Utility::GenerateLeveledList(pairValue, totalGeneratedLeveledListTargetReinserts);

        if (pairValue.generatedLeveledList)
        {
            ++totalLeveledListGenerated;
        }
        
    }

    return true;
}

bool Manager::InsertGeneratedBatchMap(ItemData& data)
{
    switch (data.targetFormType)
	{
    case Data::ITEM_FORM_TYPE:
    case Data::ARMOR_FORM_TYPE:
    case Data::LEVELED_ITEM_FORM_TYPE:
        
        return InsertIntoGeneratedBatchMap<RE::TESLevItem*>(data, itemLeveledListGenerateBatchMap);
    case Data::NPC_FORM_TYPE:
    case Data::LEVELED_NPC_FORM_TYPE:
        
        return InsertIntoGeneratedBatchMap<RE::TESLevCharacter*>(data, npcLeveledListGenerateBatchMap);
    case Data::SPELL_FORM_TYPE:
    case Data::LEVELED_SPELL_FORM_TYPE:
        
        return InsertIntoGeneratedBatchMap<RE::TESLevSpell*>(data, spellLeveledListGenerateBatchMap);
	}

    ++removedDataCounter;
    return false;
}

template<typename T> bool Manager::InsertIntoGeneratedBatchMap(ItemData& data, boost::unordered_flat_map<RE::FormID, GenerateCollection<T>>& map)
{
    // single hash lookup
    auto [iterator, wasInserted] = map.emplace(
        std::piecewise_construct, 
        std::forward_as_tuple(data.targetForm->formID), 
        std::forward_as_tuple()
    );

    iterator->second.insertVector.emplace_back(data);

    Utility::SetGeneratedLeveledListInstruction(iterator->second.instruction, data.useAll);

    return true;
}

// necessary signatures for the linker
template bool Manager::InsertIntoGeneratedBatchMap<RE::TESLevItem*>(ItemData& data, boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevItem*>>& map);
template bool Manager::InsertIntoGeneratedBatchMap<RE::TESLevCharacter*>(ItemData& data, boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevCharacter*>>& map);
template bool Manager::InsertIntoGeneratedBatchMap<RE::TESLevSpell*>(ItemData& data, boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevSpell*>>& map);

template<typename T> bool Manager::GenerateBatchMapLeveledList(boost::unordered_flat_map<RE::FormID, GenerateCollection<T>>& map)
{
    for (auto& [targetKey, pairValue] : map)
    {
        //if (!pairValue.first)
        //{

        // construct ItemData for generated leveled list and reinsert
        ItemData leveledListItemData;

        leveledListItemData.targetForm = RE::TESForm::LookupByID(targetKey);

        if (leveledListItemData.targetForm)
        {
            leveledListItemData.targetFormType = Utility::CheckFormType(leveledListItemData.targetForm);
            
            // useAll flag TRUE, will insert this generated leveled list into existing leveled lists with the useAll flag
            // (does not set set this generated leveled list's useAll flag)
            leveledListItemData.useAll = 1;

            //leveledListItemData.processCounter = Data::MAX_ENTRY_SIZE;
            leveledListItemData.chance = 100.0f;

            leveledListItemData.minCount = 1;
            leveledListItemData.maxCount = 1;

            leveledListItemData.minLevel = 1;
            leveledListItemData.maxLevel = 1;

            const GeneratedLeveledListInstruction& instruction = pairValue.instruction;
            bool countOne = ((instruction & GeneratedLeveledListInstruction::CountOne) == GeneratedLeveledListInstruction::CountOne);
            bool levelOne = ((instruction & GeneratedLeveledListInstruction::LevelOne) == GeneratedLeveledListInstruction::LevelOne);

            if (countOne && levelOne)
            {
                leveledListItemData.protocol = Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC; // don't use target count or level
            }
            else if (levelOne)
            {
                leveledListItemData.protocol = Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC_COUNT; // still use target count
            }
            else if(countOne)
            {
                leveledListItemData.protocol = Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC_LEVEL; // still use target level
            }
            else
            {
                leveledListItemData.protocol = Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC_COUNT_LEVEL; // use target count and level
            }

            pairValue.generatedLeveledList = Utility::GenerateLeveledList<T>(pairValue, totalGeneratedLeveledListTargetReinserts);

            if (pairValue.generatedLeveledList)
            {
                leveledListItemData.insertForm =  pairValue.generatedLeveledList;
                leveledListItemData.insertFormType = Utility::CheckFormType(leveledListItemData.insertForm);

                ++totalLeveledListGenerated;

                // reinsert generated leveled list
                DirectProtocol(leveledListItemData);
            }
        }

        
        //}
    }

    return true;
}

// necessary signatures for the linker
template bool Manager::GenerateBatchMapLeveledList<RE::TESLevItem*>(boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevItem*>>& map);
template bool Manager::GenerateBatchMapLeveledList<RE::TESLevCharacter*>(boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevCharacter*>>& map);
template bool Manager::GenerateBatchMapLeveledList<RE::TESLevSpell*>(boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevSpell*>>& map);

bool Manager::insertGeneratedKeywordMap(ItemData& data)
{
    switch (data.insertFormType) // has to use insertFormType since targetFormType should be keyword.
	{
    case Data::ITEM_FORM_TYPE:
    case Data::ARMOR_FORM_TYPE:
    case Data::LEVELED_ITEM_FORM_TYPE:
        
        return InsertIntoGeneratedBatchMap<RE::TESLevItem*>(data, itemLeveledListGenerateKeywordMap);
    case Data::NPC_FORM_TYPE:
    case Data::LEVELED_NPC_FORM_TYPE:
        
        return InsertIntoGeneratedBatchMap<RE::TESLevCharacter*>(data, npcLeveledListGenerateKeywordMap);
    case Data::SPELL_FORM_TYPE:
    case Data::LEVELED_SPELL_FORM_TYPE:
        
        return InsertIntoGeneratedBatchMap<RE::TESLevSpell*>(data, spellLeveledListGenerateKeywordMap);
	}

    ++removedDataCounter;
    return false;
}

bool Manager::PushGeneratedLeveledLists()
{
    PushGeneratedLeveledList<RE::TESLevItem*>(itemLeveledListGenerateBatchMap);
    PushGeneratedLeveledList<RE::TESLevCharacter*>(npcLeveledListGenerateBatchMap);
    PushGeneratedLeveledList<RE::TESLevSpell*>(spellLeveledListGenerateBatchMap);

    PushGeneratedLeveledList<RE::TESLevItem*>(itemLeveledListGenerateKeywordMap);
    PushGeneratedLeveledList<RE::TESLevCharacter*>(npcLeveledListGenerateKeywordMap);
    PushGeneratedLeveledList<RE::TESLevSpell*>(spellLeveledListGenerateKeywordMap);
    
    return true;
}

template<typename T> bool Manager::PushGeneratedLeveledList(boost::unordered_flat_map<RE::FormID, GenerateCollection<T>>& map)
{
    auto& leveledItemLists = RE::TESDataHandler::GetSingleton()->GetFormArray<std::remove_pointer_t<T>>();

    for (auto& [targetKey, pairValue] : map)
    {
        leveledItemLists.push_back(pairValue.generatedLeveledList);
    }

    return true;
}

// necessary signatures for the linker
template bool Manager::PushGeneratedLeveledList<RE::TESLevItem*>(boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevItem*>>& map);
template bool Manager::PushGeneratedLeveledList<RE::TESLevCharacter*>(boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevCharacter*>>& map);
template bool Manager::PushGeneratedLeveledList<RE::TESLevSpell*>(boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevSpell*>>& map);

template<typename T> bool Manager::SafeCircularInsertionWrapper(const T* insert, const T* list)
{
    if (!insert || !list || (insert == list))
    {
        return false;
    }

    bool value = true;

    for (std::uint8_t i = static_cast<std::uint8_t>(0); (i < list->numEntries) && (i < list->entries.size()); ++i)
    {
        T* entry = list->entries[i].form ? list->entries[i].form->As<T>() : nullptr;
        /*
        if (entry == insert)
        {
            value = false;
        }*/
        if (entry)
        {
            value = value && !SafeCircularInsertion(insert, entry, list);
        }
    }

    return value;
}

// necessary signatures for the linker
template bool Manager::SafeCircularInsertionWrapper<RE::TESLevItem>(const RE::TESLevItem* insert, const RE::TESLevItem* list);
template bool Manager::SafeCircularInsertionWrapper<RE::TESLevCharacter>(const RE::TESLevCharacter* insert, const RE::TESLevCharacter* list);
template bool Manager::SafeCircularInsertionWrapper<RE::TESLevSpell>(const RE::TESLevSpell* insert, const RE::TESLevSpell* list);

template<typename T> bool Manager::SafeCircularInsertion(const T* insert, const T* list, const T* entryList)
{
    if (list)
    {

    }
    
    return true;
}

// necessary signatures for the linker
template bool Manager::SafeCircularInsertion<RE::TESLevItem>(const RE::TESLevItem* insert, const RE::TESLevItem* list, const RE::TESLevItem* entryList);
template bool Manager::SafeCircularInsertion<RE::TESLevCharacter>(const RE::TESLevCharacter* insert, const RE::TESLevCharacter* list, const RE::TESLevCharacter* entryList);
template bool Manager::SafeCircularInsertion<RE::TESLevSpell>(const RE::TESLevSpell* insert, const RE::TESLevSpell* list, const RE::TESLevSpell* entryList);