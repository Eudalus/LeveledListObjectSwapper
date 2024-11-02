#pragma once

namespace Data
{
	// ----- FORM TYPE -----

	inline const std::int8_t INVALID_FORM_TYPE = 0;

    // item
    inline const std::int8_t ITEM_FORM_TYPE = 1;
    inline const std::int8_t LEVELED_ITEM_FORM_TYPE = 2;

    // npc
    inline const std::int8_t NPC_FORM_TYPE = 3;
    inline const std::int8_t LEVELED_NPC_FORM_TYPE = 4;

    // spell
    inline const std::int8_t SPELL_FORM_TYPE = 5;
    inline const std::int8_t LEVELED_SPELL_FORM_TYPE = 6;

    // ----- PROTOCOL -----

    inline const std::uint16_t DEFAULT_PROTOCAL_VALUE = 0;

    // needs to be consistent with existing protocols
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_MIN = 0;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_MAX = 3;

    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_MIN = 50;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_MAX = 93;

    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_MIN = 100;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_MAX = 104;

    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_MIN = 150;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_MAX = 154;

    // create and insert leveled lists with items single?

    // create and insert leveled lists with items multi?

    // create and insert leveled lists with items single, with removal?

    // create and insert leveled lists with items multi, with removal?
}

