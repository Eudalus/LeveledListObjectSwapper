#pragma once

class ItemData
{
public:
    RE::FormID targetFormID;
    RE::FormID insertFormID;
    std::string targetEditorID;
    std::string insertEditorID;
    std::string targetPlugin;
    std::string insertPlugin;
    std::uint16_t minCount, maxCount;
    std::uint16_t minLevel, maxLevel;
    float chance;
    std::uint8_t protocol;
    std::uint32_t priority;
    std::uint8_t useAll;
    RE::TESForm targetForm;
    RE::TESForm insertForm;

    bool operator<(const ItemData &other);

    bool operator==(const ItemData &other);

    bool operator>(const ItemData &other);

    bool operator!=(const ItemData &other);

    bool operator<=(const ItemData &other);

    bool operator>=(const ItemData &other);
};