#pragma once

namespace Data
{
	// ----- FORM TYPE -----

	inline const std::int8_t INVALID_FORM_TYPE = 0;

    // item
    inline const std::int8_t ITEM_FORM_TYPE = 1;
    inline const std::int8_t ARMOR_FORM_TYPE = 2;
    inline const std::int8_t LEVELED_ITEM_FORM_TYPE = 3;

    // npc
    inline const std::int8_t NPC_FORM_TYPE = 4;
    inline const std::int8_t LEVELED_NPC_FORM_TYPE = 5;

    // spell
    inline const std::int8_t SPELL_FORM_TYPE = 6;
    inline const std::int8_t LEVELED_SPELL_FORM_TYPE = 7;

    // keyword
    inline const std::int8_t KEYWORD_FORM_TYPE = 8;

    // ----- PROTOCOL -----

    inline const std::uint16_t DEFAULT_PROTOCAL_VALUE = 0;

    // needs to be consistent with existing protocols
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_MIN = 0;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_BASIC = 0;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_BASIC_COUNT = 1;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_BASIC_LEVEL = 2;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_BASIC_COUNT_LEVEL = 3;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_MAX = 3;

    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_MIN = 50;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_BASIC = 50;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_BASIC_COUNT = 51;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_BASIC_LEVEL = 52;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_BASIC_COUNT_LEVEL = 53;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_MAX = 53;

    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_MIN = 100;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_BASIC = 100;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_BASIC_COUNT = 101;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_BASIC_LEVEL = 102;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_BASIC_COUNT_LEVEL = 103;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_NO_INSERT_REMOVE = 104;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_MAX = 104;

    // remove, but replace with a generated leveled list?
    // inline const std::uint16_t VALID_SINGLE_PROTOCOL_SWAP_BASIC = 120
    // add pair to batch maps?

    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_MIN = 150;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_BASIC = 150;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_BASIC_COUNT = 151;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_BASIC_LEVEL = 152;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_BASIC_COUNT_LEVEL = 153;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_NO_INSERT_REMOVE = 154;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_MAX = 154;

    // inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_BASIC = 170
    
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_MIN = 200;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_BASIC = 200;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_MAX = 200;

    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MIN = 300;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_BASIC = 300;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MAX = 300;

    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MIN = 350;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_BASIC = 350;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MAX = 350;

    // remove, but replace with a generated leveled list?
    inline const std::uint16_t VALID_MULTI_PROTOCOL_OUTFIT_SWAP_MIN = 450;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_OUTFIT_SWAP_BASIC = 450;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_OUTFIT_SWAP_MAX = 450;

    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_INSERT_MIN = 500;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_INSERT_MAX = 500;

    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_INSERT_MIN = 550;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_INSERT_MAX = 550;

    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_MIN = 600;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_MAX = 600;

    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_MIN = 650;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_MAX = 650;

    // remove, but replace with a generated leveled list?
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_SWAP_MIN = 700;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_SWAP_MAX = 700;

    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_SWAP_MIN = 750;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_SWAP_MAX = 750;

    inline const std::uint16_t VALID_SINGLE_PROTOCOL_KEYWORD_REMOVE_MIN = 900;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_KEYWORD_REMOVE = 900;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_KEYWORD_REMOVE_MAX = 900;

    inline const std::uint16_t VALID_MULTI_PROTOCOL_KEYWORD_REMOVE_MIN = 950;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_KEYWORD_REMOVE = 950;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_KEYWORD_REMOVE_MAX = 950;

    // ----- LEVELED LIST ENTRY -----
    inline const std::size_t MAX_ENTRY_SIZE = 255; // you'd think 256, but nope
}

