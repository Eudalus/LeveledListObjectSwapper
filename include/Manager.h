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

    template<typename T> bool ProcessBatchLeveledList(const RE::FormType& formType, std::unordered_map<RE::FormID, std::vector<ItemData>>& map);
    template<typename T> bool ProcessFocusLeveledList(const RE::FormType& formType, std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>>& map);

    bool DirectProtocol(ItemData& data);
    bool InsertIntoBatchMap(ItemData& data); // calls InsertIntoCommonMap
    bool InsertIntoFocusMapAdd(ItemData& data); // calls InsertIntoWeirdMap
    bool InsertIntoFocusMapRemove(ItemData& data); // calls InsertIntoWeirdMap
    bool InsertIntoCommonMap(ItemData& data, std::unordered_map<RE::FormID, std::vector<ItemData>>& map);
    bool InsertIntoWeirdMapAdd(ItemData& data, std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>>& map);
    bool InsertIntoWeirdMapRemove(ItemData& data, std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>>& map);

    // Note that insertBufferElements is not capacity, Data::MAX_ENTRY_SIZE (255) will be array max capacity
    bool ProcessBatchProtocol(ItemData& data, RE::LEVELED_OBJECT& originalObject, std::size_t& insertBufferElements, SmallerLeveledObject* insertBuffer, bool& keepOriginal, std::vector<ItemData*>& resetVector);
    bool ProcessFocusProtocolAdd(ItemData& data, RE::LEVELED_OBJECT& originalObject, std::size_t& insertBufferElements, SmallerLeveledObject* insertBuffer);
    bool ProcessFocusProtocolRemove(SmallerItemData& data, bool& keepOriginal);

    void InsertLeveledListBuffers(const std::size_t insertBufferElements, SmallerLeveledObject* insertBuffer, const std::size_t originalBufferElements, SmallerLeveledObject* originalBuffer, RE::SimpleArray<RE::LEVELED_OBJECT>& entries);
    void InsertLeveledListVectorBuffer(const std::size_t insertLimit, std::vector<ItemData>& insertDataVector, const std::size_t originalBufferElements, SmallerLeveledObject* originalBuffer, RE::SimpleArray<RE::LEVELED_OBJECT>& entries);

    // ----- BATCH MAPS -----
    // batch inserts with item targets, may contain leveled lists targets based on item data protocol
    std::unordered_map<RE::FormID, std::vector<ItemData>> itemMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> npcMap;
    std::unordered_map<RE::FormID, std::vector<ItemData>> spellMap;

    // ----- FOCUS MAPS -----
    // specific inserts with leveled list targets
    // key is target leveled list FormID, value pair first -> std::vector<ItemData> is direct inserts into leveled list, value pair second -> std::unordered_map<FormID, SmallerItemData> needs to loop over leveled list and determine if any items inside match secondary INSERT key for removal
    std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>> itemLeveledHybridMap;
    std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>> npcLeveledHybridMap;
    std::unordered_map<RE::FormID, std::pair<std::vector<ItemData>, std::unordered_map<RE::FormID, std::vector<SmallerItemData>>>> spellLeveledHybridMap;

    std::default_random_engine randomEngine;

    std::size_t removedDataCounter = 0; // bad data from ini file
    std::size_t wrongDataCounter = 0; // good data from ini file, but can't insert into target

    // number of leveled lists edited in focus
    std::size_t uniqueListFocusModified = 0;

    // number of leveled lists edited in batch
    std::size_t uniqueListBatchModified = 0;

    // number of times a new item was inserted into a leveled list
    std::size_t totalListInserts = 0;

    // number of times an original item was removed from a leveled list
    std::size_t totalListRemovals = 0;

    // number of times that were skipped due to ItemData chance
    std::size_t totalListChanceSkips = 0;

    // number of times that leveled lists modifications were skipped due to UseAll flag
    std::size_t totalListUseAllSkips = 0;

    // number of keys in all maps
    std::size_t totalTargetSize = 0;

    // number of ItemData in all map vectors
    std::size_t totalDataSize = 0;
};