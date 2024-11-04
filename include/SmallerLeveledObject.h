#pragma once

// 16 bytes with alignment vs original 24 bytes RE::LEVELED_OBJECT
class SmallerLeveledObject
{
public:
    RE::TESForm* form;
    std::uint16_t count;
    std::uint16_t level;
};