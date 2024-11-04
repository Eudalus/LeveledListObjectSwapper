#include "Manager.h"
#include <SimpleIni.h>
#include "Utility.h"
#include "Data.h"
#include <algorithm>>

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
    const std::string DEFAULT_PRIORITY_STRING = "1000";
    const std::string DEFAULT_USE_ALL_STRING = "0";

    const unsigned int DEFAULT_COUNT_VALUE = 1;
    const unsigned int DEFAULT_LEVEL_VALUE = 1;
    const float DEFAULT_CHANCE_VALUE = 100.0;
    const float DEFAULT_CHANCE_MIN = 0.0f;
    const float DEFAULT_CHANCE_MAX = 100.0f;
    const unsigned int DEFAULT_PROTOCOL_VALUE = 0;
    const unsigned int DEFAULT_PRIORITY_VALUE = 1000;
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
            logger::info("READ SECTION: {}", section);

            Utility::GetIniValue(ini, targetString, section, "Target",
                              DEFAULT_TARGET_STRING);
            logger::info("Target: {}", targetString);

            currentData.targetForm = Utility::AcquireForm(targetString, targetEditorID, targetPlugin, targetFormID);

            currentData.targetFormType = Utility::CheckFormType(currentData.targetForm);

            // bypass all data under current section if target is not valid
            if (!currentData.targetForm || currentData.targetFormType == Data::INVALID_FORM_TYPE)
            {
                ++removedDataCounter;
                continue;
            }

            Utility::GetIniValue(ini, insertString, section, "Insert",
                              DEFAULT_INSERT_STRING);
            logger::info("Insert: {}", insertString);

            currentData.insertForm = Utility::AcquireForm(insertString, insertEditorID, insertPlugin, insertFormID);

            currentData.insertFormType = Utility::CheckFormType(currentData.insertForm);

            Utility::GetIniValue(ini, protocolString, section, "Protocol",
                              DEFAULT_PROTOCOL_STRING);
            logger::info("Protocol: {}", protocolString);

            //currentData.protocol = Utility::ClampProtocol(Utility::StringToUnInt(protocolString, DEFAULT_PROTOCOL_VALUE));
            currentData.protocol = Utility::StringToUnInt(protocolString, DEFAULT_PROTOCOL_VALUE);

            Utility::GetIniValue(ini, countString, section, "Count", DEFAULT_COUNT_STRING);
            logger::info("Count: {}", countString);

            // pass by reference assignment
            Utility::AcquireRangeData(countString, currentData.minCount, currentData.maxCount, DEFAULT_COUNT_VALUE);

            Utility::GetIniValue(ini, levelString, section, "Level",
                              DEFAULT_LEVEL_STRING);
            logger::info("Level: {}", levelString);

            // pass by reference assignment
            Utility::AcquireRangeData(levelString, currentData.minLevel, currentData.maxLevel, DEFAULT_LEVEL_VALUE);

            Utility::GetIniValue(ini, chanceString, section, "Chance",
                              DEFAULT_CHANCE_STRING);
            logger::info("Chance: {}", chanceString);

            currentData.chance = std::clamp(
                Utility::StringToFloat(chanceString, DEFAULT_CHANCE_VALUE),
                DEFAULT_CHANCE_MIN, DEFAULT_CHANCE_MAX);

            Utility::GetIniValue(ini, priorityString, section, "Priority",
                              DEFAULT_PRIORITY_STRING);
            logger::info("Priority: {}", priorityString);

            currentData.priority = Utility::StringToUnInt(priorityString, DEFAULT_PRIORITY_VALUE);

            Utility::GetIniValue(ini, useAllString, section, "UseAll",
                              DEFAULT_USE_ALL_STRING);
            logger::info("UseAll: {}", useAllString);

            currentData.useAll = Utility::StringToUnInt(useAllString, DEFAULT_USE_ALL_VALUE);

            DirectProtocol(currentData);

            //logger::info("READ COMPLETE");
        }
    }

    logger::info("{:*^30}", "RESULTS");

    logger::info("{} valid data found", totalDataSize);
    logger::info("{} invalid data removed", removedDataCounter);

    logger::info("{:*^30}", "INFO");

    SortMapData();

    return totalDataSize >= 1;
}

bool Manager::ProcessLeveledItem()
{
    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    const auto& lists = dataHandler->GetFormArray(RE::FormType::LeveledItem);
    
    const RE::BSTArrayBase::size_type numberOfLists = lists.size();

    RE::SimpleArray<RE::LEVELED_OBJECT>::size_type oldListSize, newListSize;
    std::size_t currentItemsSize, insertItemsSize;
    bool shouldInsert, ongoing;
    bool resizePending;
    bool keepOriginal;
    
    std::vector<RE::LEVELED_OBJECT> insertItems;
    insertItems.reserve(Data::MAX_ENTRY_SIZE);

    RE::LEVELED_OBJECT originalBuffer[Data::MAX_ENTRY_SIZE];
    RE::LEVELED_OBJECT insertBuffer[Data::MAX_ENTRY_SIZE];

    // not to be confused with capacity
    size_t originalBufferElements = 0;
    size_t insertBufferElements = 0;

    std::srand(std::time(NULL));

    std::default_random_engine randomEngine;
    std::uniform_real_distribution<float> randomDistributor(0.0f, 99.99f);

    // check for leveled list targets

    // check list entries for batch inserts
    for (RE::BSTArrayBase::size_type i = 0; i < numberOfLists; ++i)
    {
        RE::TESLevItem *currentList = lists[i] ? lists[i]->As<RE::TESLevItem>() : nullptr;

        if (currentList && (currentList->numEntries < Data::MAX_ENTRY_SIZE)) // remove size check here, will interfere with removal or swap
        {
            shouldInsert = false;
            ongoing = true;
            resizePending = false;

            RE::SimpleArray<RE::LEVELED_OBJECT>& currentEntries = currentList->entries;
            oldListSize = currentEntries.size();

            for (RE::SimpleArray<RE::LEVELED_OBJECT>::size_type j = 0; j < oldListSize && ongoing; ++j)
            {
                keepOriginal = true;
                auto currentForm = currentEntries[j].form;

                if (currentForm)
                {
                    if (auto mapIterator = itemMap.find(currentForm->formID); mapIterator != itemMap.end()) // if statement variable declaration is cruise control for cool
                    {
                        std::vector<ItemData> &currentItems = mapIterator->second;

                        currentItemsSize = std::min(currentItems.size(), Data::MAX_ENTRY_SIZE);

                        if (currentItemsSize > 0)
                        {
                            insertItemsSize = insertItems.size();

                            // use buffer elements instead
                            for (size_t k = 0; k < currentItemsSize && ((insertItemsSize + oldListSize) < Data::MAX_ENTRY_SIZE) && ongoing; ++k)
                            {
                                if (currentItems[k].processCounter > 0)
                                {
                                    if (randomDistributor(randomEngine) < currentItems[k].chance)
                                    {
                                        // handle protocol here
                                        if (ProcessProtocol(currentItems[k], currentEntries[j], insertBufferElements, insertBuffer, keepOriginal))
                                        {
                                            resizePending = true;
                                        }

                                        auto insertForm = currentItems[k].insertForm;

                                        if (insertForm)// && currentList->GetCanContainFormsOfType(insertForm->GetFormType()))
                                        {
                                            insertItems.emplace_back(RE::LEVELED_OBJECT(insertForm, (rand() % (currentItems[k].maxCount - currentItems[k].minCount + 1)) + currentItems[k].minCount, (rand() % (currentItems[k].maxLevel - currentItems[k].minLevel + 1)) + currentItems[k].minLevel, 0, nullptr));
                                            insertItemsSize = insertItems.size();
                                            shouldInsert = true;
                                            if (insertItemsSize + oldListSize >= Data::MAX_ENTRY_SIZE)
                                            {
                                                // won't be able to insert more items into list, lets 2nd level for loop terminate early
                                                ongoing = false;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        ++totalLeveledItemChanceSkips;
                                    }
                                }
                            }
                        }
                    }
                }

                if (keepOriginal)
                {
                    // copy original object data into originalBuffer[originalBufferElements]
                    // ++originalBufferElements;
                }
            }

            // change to resizePending, don't use insertItems size check
            if (shouldInsert && (!insertItems.empty())) // if true, target item found. resize and insert
            {
                insertItemsSize = insertItems.size(); // paranoid insurance

                // newListSize guaranteed 255 or less
                newListSize = std::clamp(oldListSize + insertItemsSize, (std::size_t)0, Data::MAX_ENTRY_SIZE);

                currentList->numEntries = newListSize;

                // should handle reallocating and moving existing data
                currentEntries.resize(newListSize);

                // insert data from insertItems to leveled list entries
                for (RE::SimpleArray<RE::LEVELED_OBJECT>::size_type m = oldListSize, z = 0; m < newListSize; ++m, ++z) // add (&& z < insertItems.size()) to condition ?
                {
                    auto& appendForm = currentEntries[m];
                    auto& itemForm = insertItems[z];

                    appendForm.count = itemForm.count;
                    appendForm.level = itemForm.level;
                    appendForm.form = itemForm.form;
                    //appendForm.itemExtra = itemForm.itemExtra;
                }

                totalLeveledItemInserts += insertItemsSize;
                ++uniqueLeveledItemInserts;

                // prepare for next list
                insertItems.clear();
                //insertItemsSize = 0; // unnecessary, will reassign before usage
                insertItems.reserve(Data::MAX_ENTRY_SIZE);

                Utility::SortLeveledListArrayByLevel(currentEntries);

                // reset process counters for next loop here
                // 



                logger::info("{} OLD LIST SIZE", oldListSize);
                logger::info("{} NEW LIST SIZE", newListSize);

                for (int debugIterator = 0; debugIterator < currentList->numEntries; ++debugIterator)
                {
                    logger::info("{} NEW LIST --- FORM ID: {} --- COUNT: {} --- LEVEL: {} --- PADDING: {}", debugIterator, std::format("{:x}", currentList->entries[debugIterator].form->formID), currentList->entries[debugIterator].count, currentList->entries[debugIterator].level, currentList->entries[debugIterator].pad0C);
                    
                    if (currentEntries[debugIterator].itemExtra)
                    {
                        logger::info("{} --- HEALTH MULT         : {}", debugIterator, currentEntries[debugIterator].itemExtra->healthMult);
                        logger::info("{} --- RANK                : {}", debugIterator, currentEntries[debugIterator].itemExtra->conditional.rank);

                        if (currentEntries[debugIterator].itemExtra->conditional.global)
                        {
                            logger::info("{} --- VALUE           : {}", debugIterator, currentEntries[debugIterator].itemExtra->conditional.global->value);
                        }

                        if (currentEntries[debugIterator].itemExtra->owner) {
                            logger::info("{} --- OWNER FORM ID   : {}", debugIterator, std::format("{:x}", currentEntries[debugIterator].itemExtra->owner->formID));
                        }
                    }
                   
                    logger::info("{} --- COMPLETE", debugIterator);
                }
            }
        }
    }

    logger::info("{} unique leveled item lists inserted into", uniqueLeveledItemInserts);
    logger::info("{} total insertions into leveled item lists", totalLeveledItemInserts);
    logger::info("{} total leveled item inserts randomly chance skipped", totalLeveledItemChanceSkips);

    return false;
}

bool Manager::ProcessLeveledNPC()
{
    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    const auto& lists = dataHandler->GetFormArray(RE::FormType::LeveledNPC);

    return false;
}

bool Manager::ProcessLeveledSpell()
{
    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    const auto& lists = dataHandler->GetFormArray(RE::FormType::LeveledSpell);

    return false;
}

void Manager::SortMapData()
{
    for (auto& [key, value] : itemMap)
    {
        std::sort(value.begin(), value.end());
    }

    for (auto& [key, value] : npcMap)
    {
        std::sort(value.begin(), value.end());
    }

    for (auto& [key, value] : spellMap)
    {
        std::sort(value.begin(), value.end());
    }

    for (auto& [key, value] : itemLeveledMap)
    {
        std::sort(value.begin(), value.end());
    }

    for (auto& [key, value] : npcLeveledMap)
    {
        std::sort(value.begin(), value.end());
    }

    for (auto& [key, value] : spellLeveledMap)
    {
        std::sort(value.begin(), value.end());
    }
}

bool Manager::DirectProtocol(ItemData& data)
{
    const auto protocol = data.protocol;

    if (Utility::CheckCompatibleFormTypes(data.insertFormType, data.targetFormType)) // check if form types are compatible
    {
        if ((protocol >= Data::VALID_SINGLE_PROTOCOL_INSERT_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_INSERT_MAX))
        {
            data.processCounter = 1;
            InsertIntoBatchMap(data);
            return true;
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_INSERT_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_INSERT_MAX))
        {
            data.processCounter = Data::MAX_ENTRY_SIZE;
            InsertIntoBatchMap(data);
            return true;
        }
        else if ((protocol >= Data::VALID_SINGLE_PROTOCOL_REMOVE_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_REMOVE_MAX))
        {
            data.processCounter = 1;
            InsertIntoBatchMap(data);
            return true;
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_REMOVE_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_REMOVE_MAX))
        {
            data.processCounter = Data::MAX_ENTRY_SIZE;
            InsertIntoBatchMap(data);
            return true;
        }
        else if ((protocol >= Data::VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_MAX))
        {
            data.processCounter = 1;
            InsertIntoFocusMap(data);
            return true;
        }
        else if ((protocol >= Data::VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MIN) && (protocol <= Data::VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MAX))
        {
            data.processCounter = 1;
            InsertIntoFocusMap(data);
            return true;
        }
        else if ((protocol >= Data::VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MIN) && (protocol <= Data::VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MAX))
        {
            data.processCounter = Data::MAX_ENTRY_SIZE;
            InsertIntoFocusMap(data);
            return true;
        }
    }
    else // form types are not compatible
    {
        // check to see if protocol doesn't require valid insert
        // target already confirmed beforehand in LoadData, add to appropriate map
        switch (protocol)
	    {
		case Data::VALID_SINGLE_PROTOCOL_NO_INSERT_REMOVE:
            data.processCounter = 1;
            InsertIntoBatchMap(data);
            return true;
        case Data::VALID_MULTI_PROTOCOL_NO_INSERT_REMOVE:
            data.processCounter = Data::MAX_ENTRY_SIZE;
            InsertIntoBatchMap(data);
            return true;
	    }
    }

    ++removedDataCounter;
    return false;
}

bool Manager::InsertIntoBatchMap(ItemData& data)
{
    switch (data.insertFormType)
	{
    case Data::ITEM_FORM_TYPE:
    case Data::LEVELED_ITEM_FORM_TYPE:
        InsertIntoMap(data, itemMap);
        return true;
    case Data::NPC_FORM_TYPE:
    case Data::LEVELED_NPC_FORM_TYPE:
        InsertIntoMap(data, npcMap);
        return true;
    case Data::SPELL_FORM_TYPE:
    case Data::LEVELED_SPELL_FORM_TYPE:
        InsertIntoMap(data, spellMap);
        return true;
	}

    return false;
}

bool Manager::InsertIntoFocusMap(ItemData& data)
{
    switch (data.insertFormType)
	{
    case Data::ITEM_FORM_TYPE:
    case Data::LEVELED_ITEM_FORM_TYPE:
        InsertIntoMap(data, itemLeveledMap);
        return true;
    case Data::NPC_FORM_TYPE:
    case Data::LEVELED_NPC_FORM_TYPE:
        InsertIntoMap(data, npcLeveledMap);
        return true;
    case Data::SPELL_FORM_TYPE:
    case Data::LEVELED_SPELL_FORM_TYPE:
        InsertIntoMap(data, spellLeveledMap);
        return true;
	}

    return false;
}

bool Manager::InsertIntoMap(ItemData& data, std::unordered_map<RE::FormID, std::vector<ItemData>>& map)
{
    if (auto mapIterator = map.find(data.targetForm->formID); mapIterator != map.end()) // if statement variable declaration is cruise control for cool
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
/// <param name="originalBufferElements"> number of elements in originalBuffer, not to be confused with max capacity (255) </param>
/// <param name="originalBuffer"> array of LEVELED_OBJECTS that will receive originalObject if data's protocol doesn't remove originalObject </param>
/// <returns> true if an insertion or removal occurred, otherwise false </returns>
bool Manager::ProcessProtocol(ItemData& data, RE::LEVELED_OBJECT& originalObject, std::size_t& insertBufferElements, RE::LEVELED_OBJECT* insertBuffer, bool& keepOriginal)
{
    switch (data.protocol)
	{
        // ----- BATCH MAPS -----
        // 0-49
		case Data::VALID_SINGLE_PROTOCOL_INSERT_BASIC:
            // insert data
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;
            insertBuffer[insertBufferElements].form = data.insertForm;

            // check extra data
            // check originalObject extra data and use on insert?

            data.processCounter = 0;

            ++insertBufferElements;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_INSERT_BASIC_COUNT:
            // insert data
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;
            insertBuffer[insertBufferElements].form = data.insertForm;

            // check extra data
            // check originalObject extra data and use on insert?

            data.processCounter = 0;
            
            ++insertBufferElements;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_INSERT_BASIC_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = originalObject.level;
            insertBuffer[insertBufferElements].form = data.insertForm;

            // check extra data
            // check originalObject extra data and use on insert?

            data.processCounter = 0;

            ++insertBufferElements;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_INSERT_BASIC_COUNT_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = originalObject.level;
            insertBuffer[insertBufferElements].form = data.insertForm;

            // check extra data
            // check originalObject extra data and use on insert?

            data.processCounter = 0;

            ++insertBufferElements;
            return true;
        // 50-99
        case Data::VALID_MULTI_PROTOCOL_INSERT_BASIC:
            // insert data
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;
            insertBuffer[insertBufferElements].form = data.insertForm;

            // check extra data
            // check originalObject extra data and use on insert?

            ++insertBufferElements;
            return true;
        case Data::VALID_MULTI_PROTOCOL_INSERT_BASIC_COUNT:
            // insert data
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;
            insertBuffer[insertBufferElements].form = data.insertForm;

            // check extra data
            // check originalObject extra data and use on insert?

            ++insertBufferElements;
            return true;
        case Data::VALID_MULTI_PROTOCOL_INSERT_BASIC_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = originalObject.level;
            insertBuffer[insertBufferElements].form = data.insertForm;

            // check extra data
            // check originalObject extra data and use on insert?

            ++insertBufferElements;
            return true;
        case Data::VALID_MULTI_PROTOCOL_INSERT_BASIC_COUNT_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = originalObject.level;
            insertBuffer[insertBufferElements].form = data.insertForm;

            // check extra data
            // check originalObject extra data and use on insert?

            ++insertBufferElements;
            return true;
        // 100-149
        case Data::VALID_SINGLE_PROTOCOL_REMOVE_BASIC:
            // insert data
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;
            insertBuffer[insertBufferElements].form = data.insertForm;

            data.processCounter = 0;

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_REMOVE_BASIC_COUNT:
            // insert data
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;
            insertBuffer[insertBufferElements].form = data.insertForm;

            data.processCounter = 0;

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_REMOVE_BASIC_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = originalObject.level;
            insertBuffer[insertBufferElements].form = data.insertForm;

            data.processCounter = 0;

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_REMOVE_BASIC_COUNT_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = originalObject.level;
            insertBuffer[insertBufferElements].form = data.insertForm;

            data.processCounter = 0;

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_SINGLE_PROTOCOL_NO_INSERT_REMOVE:

            data.processCounter = 0;
            keepOriginal = false;
            return true;
        // 150-199
        case Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC:
            // insert data
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;
            insertBuffer[insertBufferElements].form = data.insertForm;

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC_COUNT:
            // insert data
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = (rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel;
            insertBuffer[insertBufferElements].form = data.insertForm;

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].count = (rand() % (data.maxCount - data.minCount + 1)) + data.minCount;
            insertBuffer[insertBufferElements].level = originalObject.level;
            insertBuffer[insertBufferElements].form = data.insertForm;

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_MULTI_PROTOCOL_REMOVE_BASIC_COUNT_LEVEL:
            // insert data
            insertBuffer[insertBufferElements].count = originalObject.count;
            insertBuffer[insertBufferElements].level = originalObject.level;
            insertBuffer[insertBufferElements].form = data.insertForm;

            ++insertBufferElements;

            keepOriginal = false;
            return true;
        case Data::VALID_MULTI_PROTOCOL_NO_INSERT_REMOVE:
            keepOriginal = false;
            return true;
        // ----- FOCUS MAPS -----
        // 200-249
        
        // 250-299

        // 300-349

        // 350-399
	}

    /*
    auto insertForm = currentItems[k].insertForm;

    if (insertForm)// && currentList->GetCanContainFormsOfType(insertForm->GetFormType()))
    {
        insertItems.emplace_back(RE::LEVELED_OBJECT(insertForm, (rand() % (currentItems[k].maxCount - currentItems[k].minCount + 1)) + currentItems[k].minCount, (rand() % (currentItems[k].maxLevel - currentItems[k].minLevel + 1)) + currentItems[k].minLevel, 0, nullptr));
        insertItemsSize = insertItems.size();
        shouldInsert = true;
        if (insertItemsSize + oldListSize >= Data::MAX_ENTRY_SIZE)
        {
            // won't be able to insert more items into list, lets 2nd level for loop terminate early
            ongoing = false;
        }
    }
    */

    ++wrongDataCounter;
    return false;
}