#pragma once

#include "SmallerLeveledObject.h"
#include "ItemData.h"
#include "SmallerItemData.h"
#include "OutfitItemData.h"
#include <vector>
#include "boost_unordered.hpp"
#include "ContainerDirectItemData.h"
#include "ContainerGenerateItemData.h"
#include "ContainerGenerateData.h"
#include "SmallerContainerObject.h"
#include "GeneratedLeveledListInstruction.h"
#include "GenerateCollection.h"

namespace logger = SKSE::log;

class Manager
{
public:
    bool LoadData();
    
    template<typename T> bool ProcessBatchLeveledList(const RE::FormType& formType, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& map, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& keywordMap);
    template<typename T> bool ProcessFocusLeveledList(const RE::FormType& formType, boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>>& map);
    bool ProcessBatchOutfit();
    //bool ProcessBatchContainer();
    template<typename T> bool ProcessBatchContainerLite();

    bool DirectProtocol(ItemData& data);
    bool InsertIntoBatchMap(ItemData& data); // calls InsertIntoCommonMap
    bool InsertIntoFocusMapAdd(ItemData& data); // calls InsertIntoWeirdMap
    bool InsertIntoFocusMapRemove(ItemData& data); // calls InsertIntoWeirdMap
    bool InsertIntoCommonMap(ItemData& data, boost::unordered_flat_map<RE::FormID, std::vector<ItemData>>& map);
    bool InsertIntoWeirdMapAdd(ItemData& data, boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>>& map);
    bool InsertIntoWeirdMapRemove(ItemData& data, boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>>& map);
    bool InsertIntoKeywordMap(ItemData& data);
    bool InsertIntoOutfitMap(ItemData& data);
    bool GenerateOutfitLeveledLists();
    //bool InsertIntoContainerDirectMap(ItemData& data);
    //bool InsertIntoContainerGenerateMap(ItemData& data);
    bool InsertIntoContainerGenerateMapLite(ItemData& data);
    //bool GenerateContainerLeveledLists();
    bool GenerateContainerLeveledListsLite();
    template<typename T> bool SafeCircularInsertionWrapper(const T* insert, const T* list);

    // Note that insertBufferElements is not capacity, Data::MAX_ENTRY_SIZE (255) will be array max capacity
    bool ProcessBatchProtocol(ItemData& data, RE::LEVELED_OBJECT& originalObject, std::size_t& insertBufferElements, SmallerLeveledObject* insertBuffer, bool& keepOriginal, std::vector<ItemData*>& resetVector);
    bool ProcessFocusProtocolAdd(ItemData& data, RE::LEVELED_OBJECT& originalObject, std::size_t& insertBufferElements, SmallerLeveledObject* insertBuffer);
    bool ProcessFocusProtocolRemove(SmallerItemData& data, bool& keepOriginal);
    //bool ProcessContainerBatchProtocol(ContainerDirectItemData& data, RE::ContainerObject* originalObject, std::vector<SmallerContainerObject>& insertBuffer, bool& keepOriginal, std::vector<ContainerDirectItemData*>& resetVector);

    void InsertLeveledListBuffers(const std::size_t insertBufferElements, SmallerLeveledObject* insertBuffer, const std::size_t originalBufferElements, SmallerLeveledObject* originalBuffer, RE::SimpleArray<RE::LEVELED_OBJECT>& entries);
    void InsertLeveledListVectorBuffer(const std::size_t insertLimit, std::vector<ItemData>& insertDataVector, const std::size_t originalBufferElements, SmallerLeveledObject* originalBuffer, RE::SimpleArray<RE::LEVELED_OBJECT>& entries);
    void ClearAndInsertContainer(RE::TESContainer* container, std::vector<SmallerContainerObject>& insertBuffer);

    bool InsertGeneratedBatchMap(ItemData& data);

    template<typename T> bool InsertIntoGeneratedBatchMap(ItemData& data, boost::unordered_flat_map<RE::FormID, GenerateCollection<T>>& map);
    template<typename T> bool GenerateBatchMapLeveledList(boost::unordered_flat_map<RE::FormID, GenerateCollection<T>>& map);

    bool insertGeneratedKeywordMap(ItemData& data);

    // add generated leveled lists to DataHandler
    bool PushGeneratedLeveledLists();
    template<typename T> bool PushGeneratedLeveledList(boost::unordered_flat_map<RE::FormID, GenerateCollection<T>>& map);

    bool HandleCombinedProtocol(ItemData& data);

    // ----- BATCH MAPS -----
    // batch inserts with item targets, may contain leveled lists targets based on item data protocol
    boost::unordered_flat_map<RE::FormID, std::vector<ItemData>> itemMap;
    boost::unordered_flat_map<RE::FormID, std::vector<ItemData>> npcMap;
    boost::unordered_flat_map<RE::FormID, std::vector<ItemData>> spellMap;

    // ----- FOCUS MAPS -----
    // specific inserts with leveled list targets
    // key is target leveled list FormID, value pair first -> std::vector<ItemData> is direct inserts into leveled list, value pair second -> boost::unordered_flat_map<FormID, SmallerItemData> needs to loop over leveled list and determine if any items inside match secondary INSERT key for removal
    boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>> itemLeveledHybridMap;
    boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>> npcLeveledHybridMap;
    boost::unordered_flat_map<RE::FormID, std::pair<std::vector<ItemData>, boost::unordered_flat_map<RE::FormID, std::vector<SmallerItemData>>>> spellLeveledHybridMap;

    // ----- KEYWORD MAPS -----
    boost::unordered_flat_map<RE::FormID, std::vector<ItemData>> itemKeywordMap;
    boost::unordered_flat_map<RE::FormID, std::vector<ItemData>> npcKeywordMap;
    boost::unordered_flat_map<RE::FormID, std::vector<ItemData>> spellKeywordMap;

    // ----- OUTFIT MAP -----
    //boost::unordered_flat_map<RE::FormID, std::pair<RE::TESLevItem*, std::vector<OutfitItemData>>> itemOutfitMap;
    boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevItem*>> itemOutfitMap;

    // ----- OUTFIT KEYWORD MAP -----
    //boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevItem*>> itemOutfitKeywordMap;

    // ----- CONTAINER MAP -----
    //boost::unordered_flat_map<RE::FormID, std::pair<ContainerGenerateData, std::vector<ContainerDirectItemData>>> itemContainerMap;
    boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevItem*>> itemContainerMapLite;

    // ----- CONTAINER KEYWORD MAP -----
    //boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevItem*>> itemContainerKeywordMap;

    // ----- CIRCULAR MAP -----
    // key is a leveled list nested inside value set
    boost::unordered_flat_map<RE::FormID, boost::unordered_flat_set<RE::FormID>> circularExclusionMap;

    // ----- LEVELED LIST GENERATE BATCH MAPS -----
    boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevItem*>> itemLeveledListGenerateBatchMap;
    boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevCharacter*>> npcLeveledListGenerateBatchMap;
    boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevSpell*>> spellLeveledListGenerateBatchMap;

    // ----- LEVELED LIST GENERATE KEYWORD MAPS -----
    boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevItem*>> itemLeveledListGenerateKeywordMap;
    boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevCharacter*>> npcLeveledListGenerateKeywordMap;
    boost::unordered_flat_map<RE::FormID, GenerateCollection<RE::TESLevSpell*>> spellLeveledListGenerateKeywordMap;

    std::default_random_engine randomEngine;

    std::size_t removedDataCounter = 0; // bad data from ini file
    std::size_t wrongDataCounter = 0; // good data from ini file, but can't insert into target

    // number of leveled lists edited in focus
    std::size_t uniqueListFocusModified = 0;

    // number of leveled lists edited in batch
    std::size_t uniqueListBatchModified = 0;
    std::size_t uniqueContainersBatchModified = 0;
    std::size_t uniqueOutfitBatchModified = 0;

    // number of times a new item was inserted into a leveled list
    std::size_t totalListInserts = 0;
    //std::size_t totalContainerInserts = 0;

    // number of times an original item was removed from a leveled list
    std::size_t totalListRemovals = 0;
    //std::size_t totalContainerRemovals = 0;

    // number of times that were skipped due to ItemData chance
    std::size_t totalListChanceSkips = 0;
    //size_t totalContainerChanceSkips = 0;

    // number of times that leveled lists modifications were skipped due to UseAll flag
    std::size_t totalListUseAllSkips = 0;

    // number of keys in all maps
    std::size_t totalTargetSize = 0;

    // number of ItemData in all map vectors
    std::size_t totalDataSize = 0;

    // number of generated leveled list swaps
    std::size_t totalContainerSwaps = 0;
    std::size_t totalOutfitSwaps = 0;

    // number of leveled lists generated
    size_t totalLeveledListGenerated = 0;

    // number of times generated leveled lists have target item reinserted due to repurposed use all flag
    // example:
    // Protocol=170
    // UseAll=8
    size_t totalGeneratedLeveledListTargetReinserts = 0;

private:
    template<typename T> bool SafeCircularInsertion(const T* insert, const T* list, const T* entryList);
};