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

    const unsigned int VALID_SINGLE_PROTOCOL_MIN = 0;
    const unsigned int VALID_SINGLE_PROTOCOL_MAX = 1;

    const unsigned int VALID_MULTI_PROTOCOL_MIN = 100;
    const unsigned int VALID_MULTI_PROTOCOL_MAX = 101;

    // batch inserts with item targets, may contain leveled lists targets based on item data protocol
    std::unordered_map<RE::FormID, std::vector<ItemData>> itemMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> npcMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> spellMap;

    // specific inserts with leveled list targets
    std::unordered_map<RE::FormID, std::vector<ItemData>> itemLeveledMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> npcLeveledMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> spellLeveledMap;

    std::size_t removedDataCounter = 0; // bad data from ini file
    std::size_t wrongDataCounter = 0; // good data from ini file, but can't insert into target

    std::size_t uniqueLeveledItemInserts;
    std::size_t totalLeveledItemInserts;

    std::size_t uniqueLeveledNPCInserts;
    std::size_t totalLeveledNPCInserts;

    std::size_t uniqueLeveledSpellInserts;
    std::size_t totalLeveledSpellInserts;

};