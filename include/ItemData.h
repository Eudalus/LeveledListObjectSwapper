#pragma once

class ItemData
{
public:
    std::uint16_t minCount, maxCount;
    std::uint16_t minLevel, maxLevel;
    float chance;
    std::uint16_t protocol;
    std::uint32_t priority;
    std::uint8_t useAll;
    RE::TESForm* targetForm;
    RE::TESForm* insertForm;
    std::int8_t targetFormType;
    std::int8_t insertFormType;

    bool operator<(const ItemData &other);

    bool operator==(const ItemData &other);

    bool operator>(const ItemData &other);

    bool operator!=(const ItemData &other);

    bool operator<=(const ItemData &other);

    bool operator>=(const ItemData &other);
};