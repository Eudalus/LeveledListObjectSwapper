#pragma once

// Implement AverageItemData with targetForm, targetFormType, and insertFormType removed for most cases. Would be 24 bytes vs 40
class ItemData
{
public:
    RE::TESForm* targetForm;
    RE::TESForm* insertForm;
    float chance;
    std::uint16_t minCount, maxCount;
    std::uint16_t minLevel, maxLevel;
    std::uint16_t protocol;
    std::uint8_t useAll;
    std::uint8_t processCounter;
    std::int8_t targetFormType;
    std::int8_t insertFormType;

    void CopyItemData(const ItemData& other)
    {
        targetForm = other.targetForm;
        insertForm = other.insertForm;
        chance = other.chance;
        minCount = other.minCount;
        maxCount = other.maxCount;
        minLevel = other.minLevel;
        maxLevel = other.maxLevel;
        protocol = other.protocol;
        useAll = other.useAll;
        processCounter = other.processCounter;
        targetFormType = other.targetFormType;
        insertFormType = other.insertFormType;
    }
};