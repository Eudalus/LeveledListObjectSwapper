#pragma once

#include "ItemData.h"

// 32 bytes with alignment vs original 40 with alignment, 24 with targetForm removed
class ContainerGenerateItemData
{
public:
    //RE::TESForm* targetForm;
    RE::TESForm* insertForm;
    std::uint16_t minCount, maxCount;
    std::uint16_t minLevel, maxLevel;
    std::uint16_t protocol;
    //std::int8_t targetFormType;
    //std::int8_t insertFormType;

    ContainerGenerateItemData(const ItemData& data)
    {
        //targetForm = data.targetForm;
        insertForm = data.insertForm;
        minCount = data.minCount;
        maxCount = data.maxCount;
        minLevel = data.minLevel;
        maxLevel = data.maxLevel;
        protocol = data.protocol;
        //targetFormType = data.targetFormType;
        //insertFormType = data.insertFormType;
    }
};