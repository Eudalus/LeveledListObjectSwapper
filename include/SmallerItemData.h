#pragma once

#include "ItemData.h"

// 32 bytes with alignment vs original 40 with alignment
class SmallerItemData
{
public:
    RE::TESForm* targetForm;
    RE::TESForm* insertForm;
    float chance;
    std::uint16_t protocol;
    std::uint8_t useAll;
    std::uint8_t processCounter;
    std::int8_t targetFormType;
    std::int8_t insertFormType;

    SmallerItemData(const ItemData& data)
    {
        targetForm = data.targetForm;
        insertForm = data.insertForm;
        chance = data.chance;
        protocol = data.protocol;
        useAll = data.useAll;
        processCounter = data.processCounter;
        targetFormType = data.targetFormType;
        insertFormType = data.insertFormType;
    }
};