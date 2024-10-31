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

    const unsigned int DEFAULT_COUNT_VALUE = 1;
    const unsigned int DEFAULT_LEVEL_VALUE = 1;
    const float DEFAULT_CHANCE_VALUE = 100.0;
    const float DEFAULT_CHANCE_MIN = 0.0f;
    const float DEFAULT_CHANCE_MAX = 100.0f;
    const unsigned int DEFAULT_PROTOCOL_VALUE = 0;
    const unsigned int DEFAULT_PRIORITY_VALUE = 1000;
    const unsigned int DEFAULT_USE_ALL_VALUE = 0;

    const unsigned int VALID_SINGLE_PROTOCOL_MIN = 0;
    const unsigned int VALID_SINGLE_PROTOCOL_MAX = 1;

    const unsigned int VALID_MULTI_PROTOCOL_MIN = 100;
    const unsigned int VALID_MULTI_PROTOCOL_MAX = 101;

    std::unordered_map<RE::FormID, std::vector<ItemData>> dataMap; // insert item with item targets into leveled lists
    std::unordered_map<RE::FormID, std::vector<ItemData>> listMap; // insert item with specific leveled list target

    std::size_t removedDataCounter = 0; // bad data from ini file
    std::size_t wrongDataCounter = 0; // good data from ini file, but can't insert into target

    std::size_t uniqueLeveledItemInserts;
    std::size_t totalLeveledItemInserts;

    std::size_t uniqueLeveledNPCInserts;
    std::size_t totalLeveledNPCInserts;

    std::size_t uniqueLeveledSpellInserts;
    std::size_t totalLeveledSpellInserts;

};