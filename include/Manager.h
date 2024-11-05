#pragma once

#include "SmallerLeveledObject.h"
#include "ItemData.h"
#include "SmallerItemData.h"
#include <unordered_map>
#include <vector>

namespace logger = SKSE::log;

class Manager
{
public:

    bool LoadData();

    template<typename T> bool ProcessBatchLeveledList(const RE::FormType& formType);

    bool DirectProtocol(ItemData& data);
    bool InsertIntoBatchMap(ItemData& data); // calls InsertIntoCommonMap
    bool InsertIntoFocusMapAdd(ItemData& data); // calls InsertIntoWeirdMap
    bool InsertIntoFocusMapRemove(ItemData& data); // calls InsertIntoWeirdMap
    bool InsertIntoCommonMap(ItemData& data, std::unordered_map<RE::FormID, std::vector<ItemData>>& map);
    bool InsertIntoWeirdMapAdd(ItemData& data, std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>>& map);
    bool InsertIntoWeirdMapRemove(ItemData& data, std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>>& map);

    // Note that insertBufferElements is not capacity, Data::MAX_ENTRY_SIZE (255) will be array max capacity
    bool ProcessBatchProtocol(ItemData& data, RE::LEVELED_OBJECT& originalObject, std::size_t& insertBufferElements, SmallerLeveledObject* insertBuffer, bool& keepOriginal, std::vector<ItemData*>& resetVector);
    bool ProcessFocusProtocol(ItemData& data, RE::LEVELED_OBJECT& originalObject, std::size_t& insertBufferElements, SmallerLeveledObject* insertBuffer, bool& keepOriginal);

    void InsertLeveledListBuffers(const std::size_t insertBufferElements, SmallerLeveledObject* insertBuffer, const std::size_t originalBufferElements, SmallerLeveledObject* originalBuffer, RE::SimpleArray<RE::LEVELED_OBJECT>& entries, const std::size_t entriesCapacity);

    // ----- BATCH MAPS -----
    // batch inserts with item targets, may contain leveled lists targets based on item data protocol
    std::unordered_map<RE::FormID, std::vector<ItemData>> itemMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> npcMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> spellMap;

    // ----- FOCUS MAPS -----
    // specific inserts with leveled list targets
    //std::unordered_map<RE::FormID, std::vector<ItemData>> itemLeveledMap;
    //std::unordered_map<RE::FormID, std::vector<ItemData>> npcLeveledMap;
    //std::unordered_map<RE::FormID, std::vector<ItemData>> spellLeveledMap;

    // key is target leveled list FormID, value pair first -> std::vector<ItemData> is direct inserts into leveled list, value pair second -> std::unordered_map<FormID, SmallerItemData> needs to loop over leveled list and determine if any items inside match secondary INSERT key for removal
    std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>> itemLeveledHybridMap;
    std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>> npcLeveledHybridMap;
    std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>> spellLeveledHybridMap;

    std::size_t removedDataCounter = 0; // bad data from ini file
    std::size_t wrongDataCounter = 0; // good data from ini file, but can't insert into target

    // number of leveled lists edited
    std::size_t uniqueLeveledItemModified = 0;
    std::size_t uniqueLeveledNPCModified = 0;
    std::size_t uniqueLeveledSpellModified = 0;

    // number of times a new item was inserted into a leveled list
    std::size_t totalLeveledItemInserts = 0;
    std::size_t totalLeveledNPCInserts = 0;
    std::size_t totalLeveledSpellInserts = 0;

    // number of times an original item was removed from a leveled list
    std::size_t totalLeveledItemRemovals = 0;
    std::size_t totalLeveledNPCRemovals = 0;
    std::size_t totalLeveledSpellRemovals = 0;

    // number of times that were skipped due to ItemData chance
    std::size_t totalLeveledItemChanceSkips = 0;
    std::size_t totalLeveledNPCChanceSkips = 0;
    std::size_t totalLeveledSpellChanceSkips = 0;

    // number of times that leveled lists modifications were skipped due to UseAll flag
    std::size_t totalLeveledItemUseAllSkips = 0;
    std::size_t totalLeveledNPCUseAllSkips = 0;
    std::size_t totalLeveledSpellUseAllSkips = 0;

    // number of keys in all maps
    std::size_t totalTargetSize = 0;

    // number of ItemData in all map vectors
    std::size_t totalDataSize = 0;
};