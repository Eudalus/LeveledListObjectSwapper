#pragma once

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
};