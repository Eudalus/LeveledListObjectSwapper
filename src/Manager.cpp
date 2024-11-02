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
    std::size_t position;

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

            Utility::AcquireRangeData(countString, currentData.minCount, currentData.maxCount, DEFAULT_COUNT_VALUE);

            Utility::GetIniValue(ini, levelString, section, "Level",
                              DEFAULT_LEVEL_STRING);
            logger::info("Level: {}", levelString);

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

            // move this logic into direct protocol
            if (itemMap.count(currentData.targetForm->formID) == 0)
            {
                itemMap.emplace(currentData.targetForm->formID, std::vector<ItemData>());
                itemMap.at(currentData.targetForm->formID).emplace_back(currentData);

                ++totalTargetSize;
                ++totalDataSize;
            }
            else
            {
                itemMap.at(currentData.targetForm->formID).emplace_back(currentData);

                ++totalDataSize;
            }

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
    
    std::vector<RE::LEVELED_OBJECT> insertItems;
    insertItems.reserve(Data::MAX_ENTRY_SIZE);

    std::srand(std::time(NULL));

    std::default_random_engine randomEngine;
    std::uniform_real_distribution<float> randomDistributor(0.0f, 100.0f);

    //randomDistributor(randomEngine)
    // check for leveled list targets

    // check list entries for batch inserts
    for (RE::BSTArrayBase::size_type i = 0; i < numberOfLists; ++i)
    {
        RE::TESLevItem *currentList = lists[i] ? lists[i]->As<RE::TESLevItem>() : nullptr;

        if (currentList && (currentList->numEntries < Data::MAX_ENTRY_SIZE))
        {
            shouldInsert = false;
            ongoing = true;

            RE::SimpleArray<RE::LEVELED_OBJECT>& currentEntries = currentList->entries;
            oldListSize = currentEntries.size();

            for (RE::SimpleArray<RE::LEVELED_OBJECT>::size_type j = 0; j < oldListSize && ongoing; ++j)
            {
                auto currentForm = currentEntries[j].form;

                if (currentForm)
                {
                    if (itemMap.count(currentForm->formID) > 0)
                    {
                        std::vector<ItemData> &currentItems = itemMap.at(currentForm->formID);

                        currentItemsSize = std::min(currentItems.size(), Data::MAX_ENTRY_SIZE);

                        if (currentItemsSize > 0)
                        {
                            shouldInsert = true;
                            insertItemsSize = insertItems.size();

                            for (size_t k = 0; k < currentItemsSize && ((insertItemsSize + oldListSize) < Data::MAX_ENTRY_SIZE) && ongoing; ++k)
                            {
                                auto insertForm = currentItems[k].insertForm;

                                if (insertForm && currentList->GetCanContainFormsOfType(insertForm->GetFormType()))
                                {
                                    // store pointer on ItemData to insert directly to reduce lookup?
                                    insertItems.emplace_back(RE::LEVELED_OBJECT(insertForm, (rand() % (currentItems[k].maxCount - currentItems[k].minCount + 1)) + currentItems[k].minCount, (rand() % (currentItems[k].maxLevel - currentItems[k].minLevel + 1)) + currentItems[k].minLevel, 0, nullptr));
                                    insertItemsSize = insertItems.size();
                                    if (insertItemsSize + oldListSize >= Data::MAX_ENTRY_SIZE)
                                    {
                                        // won't be able to insert more items into list
                                        ongoing = false;
                                    }
                                }
                            }
                        }
                    }
                }
            }

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

    }
    else // form types are not compatible
    {
        // check to see if protocol doesn't require valid insert
        // target already confirmed beforehand in LoadData, add to appropriate map
        switch (protocol)
	    {
		case Data::VALID_SINGLE_PROTOCOL_NO_INSERT_REMOVE:
            data.processCounter = 1;

            return true;
        case Data::VALID_MULTI_PROTOCOL_NO_INSERT_REMOVE:
            data.processCounter = Data::MAX_ENTRY_SIZE;
            return true;
	    }
    }

    ++removedDataCounter;
    return false;
}

bool Manager::InsertIntoBatchMap(ItemData& data)
{

}

bool Manager::InsertIntoFocusMap(ItemData& data)
{

}