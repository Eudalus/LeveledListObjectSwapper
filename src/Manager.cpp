#include "Manager.h"
#include <SimpleIni.h>
#include "Utility.h"

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
    bool targetGood;
    bool insertGood;

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

            targetGood = Utility::AcquireFormDataAndCheck(targetString, currentData.targetEditorID, currentData.targetPlugin, currentData.targetFormID);

            if (!targetGood)
            {
                ++removedDataCounter;
                continue;
            }

            Utility::GetIniValue(ini, insertString, section, "Insert",
                              DEFAULT_INSERT_STRING);
            logger::info("Insert: {}", insertString);

            insertGood = Utility::AcquireFormDataAndCheck(insertString, currentData.insertEditorID, currentData.insertPlugin, currentData.insertFormID);

            if (!insertGood)
            {
                ++removedDataCounter;
                continue;
            }

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

            Utility::GetIniValue(ini, protocolString, section, "Protocol",
                              DEFAULT_PROTOCOL_STRING);
            logger::info("Protocol: {}", protocolString);

            currentData.protocol = Utility::StringToUnInt(protocolString, DEFAULT_PROTOCOL_VALUE);

            Utility::GetIniValue(ini, priorityString, section, "Priority",
                              DEFAULT_PRIORITY_STRING);
            logger::info("Priority: {}", priorityString);

            currentData.priority = Utility::StringToUnInt(priorityString, DEFAULT_PRIORITY_VALUE);

            Utility::GetIniValue(ini, useAllString, section, "UseAll",
                              DEFAULT_USE_ALL_STRING);
            logger::info("UseAll: {}", useAllString);

            currentData.useAll = Utility::StringToUnInt(useAllString, DEFAULT_USE_ALL_VALUE);

            /*
            if ((!insertGood) && (currentData.protocol != ))
            {

                continue;
            }
            */

            if (dataMap.count(currentData.targetFormID) == 0)
            {
                dataMap.emplace(currentData.targetFormID, std::vector<ItemData>());
                dataMap.at(currentData.targetFormID).emplace_back(currentData);
            }
            else
            {
                dataMap.at(currentData.targetFormID).emplace_back(currentData);
            }

            //logger::info("READ COMPLETE");
        }
    }

    logger::info("{:*^30}", "RESULTS");

    logger::info("{} valid data found", dataMap.size());
    logger::info("{} invalid data removed", removedDataCounter);

    logger::info("{:*^30}", "INFO");

    return !dataMap.empty();
}

bool Manager::ProcessLeveledItem()
{
    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    const auto& lists = dataHandler->GetFormArray(RE::FormType::LeveledItem);

    const auto size = lists.size();

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