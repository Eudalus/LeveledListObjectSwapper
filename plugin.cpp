#include "Manager.h"
#include "Hooks.h"

#include <ranges>
#include <iostream>
#include <fstream>
#include <cctype>    // std::tolower
#include <algorithm> // std::equal

// Include spdlog support for a basic file logger
// See below for more details
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

// Allows us to check if a debugger is attached (optional, see below)
#include <Windows.h>

#if defined(USING_UPKEEP_MANAGER)
#include "UpkeepManager.h"
#endif

namespace logger = SKSE::log;

void SetupLog()
{
    // Get the path to the SKSE logs folder
    // This will generally be your Documents\My Games\Skyrim Special Edition\SKSE
    //                          or Documents\My Games\Skyrim Special Edition GOG\SKSE
    auto logsFolder = SKSE::log::log_directory();

    // I really don't understand why the log_directory() might not be provided sometimes,
    // but... just incase... ?
    if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");

    // Get the name of this SKSE plugin. We will use it to name the log file.
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();

    // Generate a path to our log file
    // e.g. Documents\My Games\Skyrim Special Edition\SKSE\OurPlugin.log
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);

    // Now, use whatever you want, but spdlog comes with CommonLibSSE
    // and is the SKSE logger of choice. So you might as well use it!

    // Let's use a spdlog "basic file sink"
    // So like... just a file logger...
    // But the spdlog interface really wants a Shared Pointer to the "basic file sink"
    // So we'll make one of those!
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);

    // // Ok, but set_default_logger() specifically wants a Shared Pointer to a spdlog::logger
    // // So we'll make one of those!
    std::shared_ptr<spdlog::logger> loggerPtr;

    // Now, this is pretty cool.
    // If you want the logs to show up *inside your IDE* when you are debugging, use this code.
    // Whenever a debugger is attached, the logs are setup with an *additional* "sink" to goto
    // your IDE's debug output window.
    if (IsDebuggerPresent()) {
        auto debugLoggerPtr = std::make_shared<spdlog::sinks::msvc_sink_mt>();
        spdlog::sinks_init_list loggers{std::move(fileLoggerPtr), std::move(debugLoggerPtr)};
        loggerPtr = std::make_shared<spdlog::logger>("log", loggers);
    } else {
        // If no debugger is attached, only log to the file.
        loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    }

    // We'll give it the logger we made above. Yeah, I know, kinda redundant right? Welcome to C++
    spdlog::set_default_logger(std::move(loggerPtr));

    // Yay, let's setup spdlog now!
    // By default, let's print out *everything* including trace messages
    // You might want to do something like #ifdef NDEBUG then use trace, else use info or higher severity.
    spdlog::set_level(spdlog::level::trace);

    // This bit is important. When does spdlog WRITE to the file?
    // Make sure it does it everytime you log a message, otherwise it won't write to the file until the game exits.
    spdlog::flush_on(spdlog::level::trace);
}

void MessageHandler(SKSE::MessagingInterface::Message* a_message)
{
    /*
	if (a_message->type == SKSE::MessagingInterface::kPostLoad)
	{
        
	}
	else */if (a_message->type == SKSE::MessagingInterface::kPostPostLoad)
	{
        MergeMapperPluginAPI::GetMergeMapperInterface001();
        //InitHooks::OutfitInitItemImplHook::Hook();
        //LoadHooks::TESObjectREFRLoad3DHook::Hook();
        //LoadHooks::ActorLoad3DHook::Hook();
        
	}
    else if (a_message->type == SKSE::MessagingInterface::kDataLoaded)
    {
        std::chrono::time_point<std::chrono::system_clock> start, end;

        start = std::chrono::system_clock::now();
        
        Manager manager;
        manager.LoadData();
        //if (manager.LoadData())
        //{
            // seed random number generator used for min-max count and level
            std::srand(std::time(NULL));

            // seed random number generator used for modification chance
            manager.randomEngine.seed(std::random_device()()); // for real

            // ----- FOCUS FUNCTIONS -----
            if (!manager.itemLeveledHybridMap.empty())
            {
                manager.ProcessFocusLeveledList<RE::TESLevItem>(RE::FormType::LeveledItem, manager.itemLeveledHybridMap);
            }
            if (!manager.npcLeveledHybridMap.empty())
            {
                manager.ProcessFocusLeveledList<RE::TESLevCharacter>(RE::FormType::LeveledNPC, manager.npcLeveledHybridMap);
            }
            if (!manager.spellLeveledHybridMap.empty())
            {
                manager.ProcessFocusLeveledList<RE::TESLevSpell>(RE::FormType::LeveledSpell, manager.spellLeveledHybridMap);
            }

            // ----- GENERATED LEVELED LIST BATCH FUNCTIONS -----
            manager.GenerateBatchMapLeveledList<RE::TESLevItem*>(manager.itemLeveledListGenerateBatchMap);
            manager.GenerateBatchMapLeveledList<RE::TESLevCharacter*>(manager.npcLeveledListGenerateBatchMap);
            manager.GenerateBatchMapLeveledList<RE::TESLevSpell*>(manager.spellLeveledListGenerateBatchMap);

            // ----- GENERATED LEVELED LIST KEYWORD FUNCTIONS -----
            manager.GenerateBatchMapLeveledList<RE::TESLevItem*>(manager.itemLeveledListGenerateKeywordMap);
            manager.GenerateBatchMapLeveledList<RE::TESLevCharacter*>(manager.npcLeveledListGenerateKeywordMap);
            manager.GenerateBatchMapLeveledList<RE::TESLevSpell*>(manager.spellLeveledListGenerateKeywordMap);

            // ----- BATCH FUNCTIONS -----
            if (!manager.itemMap.empty() || !manager.itemKeywordMap.empty())
            {
                manager.ProcessBatchLeveledList<RE::TESLevItem>(RE::FormType::LeveledItem, manager.itemMap, manager.itemKeywordMap);
            }
            if (!manager.npcMap.empty() || !manager.npcKeywordMap.empty())
            {
                manager.ProcessBatchLeveledList<RE::TESLevCharacter>(RE::FormType::LeveledNPC, manager.npcMap, manager.npcKeywordMap);
            }
            if (!manager.spellMap.empty() || !manager.spellKeywordMap.empty())
            {
                manager.ProcessBatchLeveledList<RE::TESLevSpell>(RE::FormType::LeveledSpell, manager.spellMap, manager.spellKeywordMap);
            }

            // ----- OUTFIT FUNCTIONS -----
            if (manager.GenerateOutfitLeveledLists())
            {
                manager.ProcessBatchOutfit();

                #if defined(USING_UPKEEP_MANAGER)

                // implied UpkeepManager::GetSingleton()->totalOutfitItems > 0
                if (UpkeepManager::GetSingleton()->totalOutfitItems)
                {
                    LoadHooks::CharacterLoad3DHook::Hook(); // comment out to disable outfit reequipping
                }
                #endif
            }

            // ----- CONTAINER FUNCTIONS -----
            if (!manager.itemContainerMapLite.empty())
            {
                manager.GenerateContainerLeveledListsLite();
                manager.ProcessBatchContainerLite();
            }

            manager.PushGeneratedLeveledLists();

        //}
        
        logger::info("{:*^30}", "RESULTS");

        logger::info("{} valid data found", manager.totalDataSize);
        logger::info("{} invalid data removed", manager.removedDataCounter);
        logger::info("{} mismatched data skipped", manager.wrongDataCounter);

        logger::info("{:*^30}", "INFO");

        logger::info("");
        logger::info("{:*^30}", "LEVELED LISTS");
        logger::info("{} unique leveled lists batch modified", manager.uniqueListBatchModified);
        logger::info("{} unique target leveled lists modified", manager.uniqueListFocusModified);
        logger::info("{} total leveled list insertions", manager.totalListInserts);
        logger::info("{} total leveled list removals", manager.totalListRemovals);
        logger::info("{} total leveled list modifications randomly chance skipped", manager.totalListChanceSkips);
        logger::info("{} total leveled list modifications skipped due to UseAll flag", manager.totalListUseAllSkips);
        logger::info("{} total leveled lists generated for leveled lists, outfits, or containers", manager.totalLeveledListGenerated);
        logger::info("{} total target item automatically reinserted into generated leveled lists", manager.totalGeneratedLeveledListTargetReinserts);
        logger::info("");
        logger::info("{:*^30}", "OUTFITS");
        logger::info("{} unique outfits batch modified", manager.uniqueOutfitBatchModified);
        logger::info("{} total outfit generated leveled list swaps", manager.totalOutfitSwaps);
        logger::info("");
        logger::info("{:*^30}", "CONTAINERS");
        logger::info("{} unique containers batch modified", manager.uniqueContainersBatchModified);
        //logger::info("{} total container insertions", manager.totalContainerInserts);
        //logger::info("{} total container removals", manager.totalContainerRemovals);
        logger::info("{} total container generated leveled list swaps", manager.totalContainerSwaps);
        //logger::info("{} total container modifications randomly chance skipped", manager.totalListChanceSkips);

        end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;
        logger::info("{}s elapsed time", elapsed_seconds.count());
        
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SetupLog();

    SKSE::Init(skse);

    const auto messaging = SKSE::GetMessagingInterface();
    messaging->RegisterListener(MessageHandler);

    return true;
}