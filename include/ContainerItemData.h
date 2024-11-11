#pragma once

#include "ItemData.h"

class ContainerItemData
{
public:
    RE::TESForm* targetForm;
    RE::TESForm* insertForm;
    float chance;
    std::uint16_t minCount, maxCount;
    std::uint16_t protocol;
    std::uint8_t useAll;
    std::uint8_t processCounter;
    std::int8_t targetFormType;
    std::int8_t insertFormType;

    ContainerItemData(const ItemData& data)
    {
        targetForm = data.targetForm;
        insertForm = data.insertForm;
        chance = data.chance;
        minCount = data.minCount;
        maxCount = data.maxCount;
        protocol = data.protocol;
        useAll = data.useAll;
        processCounter = data.processCounter;
        targetFormType = data.targetFormType;
        insertFormType = data.insertFormType;
    }
};