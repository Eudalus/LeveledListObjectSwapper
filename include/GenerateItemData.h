#pragma once

#include "ItemData.h"

// 16 bytes with alignment, should generate ranged count and level prior 
class GenerateItemData
{
public:
	//RE::TESForm* targetForm;
    RE::TESForm* insertForm;
    //std::uint16_t minCount, maxCount;
    //std::uint16_t minLevel, maxLevel;
    //std::uint16_t protocol;

    uint16_t count;
    uint16_t level;

    //int8_t targetFormType;
    int8_t insertFormType;

    GenerateItemData(RE::TESForm* inInsertForm, uint16_t inCount, uint16_t inLevel, int8_t inInsertFormType)
    {
        insertForm = inInsertForm;
        count = inCount;
        level = inLevel;
        insertFormType = inInsertFormType;
    }

    GenerateItemData(const ItemData& data)
    {
        //targetForm = data.targetForm;
        insertForm = data.insertForm;
        //minCount = data.minCount;
        //maxCount = data.maxCount;
        //minLevel = data.minLevel;
        //maxLevel = data.maxLevel;
        //protocol = data.protocol;
        //targetFormType = data.targetFormType;

        count = ((rand() % (data.maxCount - data.minCount + 1)) + data.minCount);
        level = ((rand() % (data.maxLevel - data.minLevel + 1)) + data.minLevel);

        insertFormType = data.insertFormType;
    }
};