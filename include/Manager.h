#pragma once

#include "ItemData.h"
#include <unordered_map>
#include <vector>

namespace logger = SKSE::log;

class Manager
{
public:

    bool LoadData();
    bool ProcessLeveledItem();
    bool ProcessLeveledNPC();
    bool ProcessLeveledSpell();
    void SortMapData();
    bool DirectProtocol(ItemData& data);
    bool InsertIntoBatchMap(ItemData& data);
    bool InsertIntoFocusMap(ItemData& data);
    bool InsertIntoMap(ItemData& data, std::unordered_map<RE::FormID, std::vector<ItemData>>& map);

    // ----- BATCH MAPS -----
    // batch inserts with item targets, may contain leveled lists targets based on item data protocol
    std::unordered_map<RE::FormID, std::vector<ItemData>> itemMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> npcMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> spellMap;

    // ----- FOCUS MAPS -----
    // specific inserts with leveled list targets
    std::unordered_map<RE::FormID, std::vector<ItemData>> itemLeveledMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> npcLeveledMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> spellLeveledMap;

    std::size_t removedDataCounter = 0; // bad data from ini file
    std::size_t wrongDataCounter = 0; // good data from ini file, but can't insert into target

    std::size_t uniqueLeveledItemInserts = 0;
    std::size_t totalLeveledItemInserts = 0;

    std::size_t uniqueLeveledNPCInserts = 0;
    std::size_t totalLeveledNPCInserts = 0;

    std::size_t uniqueLeveledSpellInserts = 0;
    std::size_t totalLeveledSpellInserts = 0;

    std::size_t totalTargetSize = 0;
    std::size_t totalDataSize = 0;
};