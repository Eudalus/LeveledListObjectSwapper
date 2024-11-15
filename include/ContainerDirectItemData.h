#pragma once

#include "ItemData.h"

// 32 bytes with alignment vs original 40 with alignment, 24 with targetForm removed
class ContainerDirectItemData
{
public:
    //RE::TESForm* targetForm;
    RE::TESForm* insertForm;
    float chance;
    std::uint16_t minCount, maxCount;
    std::uint16_t protocol;
    std::uint8_t processCounter;
    //std::int8_t targetFormType;
    //std::int8_t insertFormType;

    ContainerDirectItemData(const ItemData& data)
    {
        //targetForm = data.targetForm;
        insertForm = data.insertForm;
        chance = data.chance;
        minCount = data.minCount;
        maxCount = data.maxCount;
        protocol = data.protocol;
        processCounter = data.processCounter;
        //targetFormType = data.targetFormType;
        //insertFormType = data.insertFormType;
    }
};