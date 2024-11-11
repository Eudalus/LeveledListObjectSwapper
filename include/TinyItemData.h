#pragma once

#include "ItemData.h"

// 24 bytes with alignment vs original 40 with alignment
class TinyItemData
{
public:
    RE::TESForm* targetForm;
    RE::TESForm* insertForm;
    float chance;
    std::uint16_t protocol;
    //std::uint8_t processCounter; // shouldn't be necessary since outfits shouldn't have the same item twice, right?

    TinyItemData(const ItemData& data)
    {
        targetForm = data.targetForm;
        insertForm = data.insertForm;
        chance = data.chance;
        protocol = data.protocol;
        //processCounter = data.processCounter;
    }
};