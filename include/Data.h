#pragma once

namespace Data
{
    // ----- GENERATED LEVELED LIST FLAG -----
    inline const RE::TESLeveledList::Flag GENERATED_LEVELED_LIST_FLAGS = static_cast<RE::TESLeveledList::Flag>(static_cast<uint8_t>(RE::TESLeveledList::Flag::kCalculateFromAllLevelsLTOrEqPCLevel) | static_cast<uint8_t>(RE::TESLeveledList::Flag::kCalculateForEachItemInCount));

	// ----- LEVELED LIST ENTRY -----
    inline const std::size_t MAX_ENTRY_SIZE = 255; // you'd think 256, but nope

    // ----- MAX EXTRA DATA LOOKUP -----
    #if declared(USING_UPKEEP_MANAGER)
    inline const std::size_t MAX_OUTFIT_LOOKUP_SIZE = 100;
    #endif
    //inline const std::size_t MAX_CONTAINER_LOOKUP_SIZE = 30;

    // ----- MAX CONTAINER MODIFY SIZE -----
    inline const uint32_t MAX_CONTAINER_MODIFY_SIZE = 64; // will avoid massive QA chests

    // ----- PROTOCOL -----

    inline const std::uint16_t DEFAULT_PROTOCAL_VALUE = 0;

    // ----- BATCH LEVELED LIST SINGLE INSERTION PROTOCOLS -----

    // needs to be consistent with existing protocols
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_MIN = 0;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_BASIC = 0;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_BASIC_COUNT = 1;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_BASIC_LEVEL = 2;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_BASIC_COUNT_LEVEL = 3;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_MAX = 3;

    /*
    // generate new leveled list with insert items and insert alongside target once
    inline const std::int16_t VALID_SINGLE_PROTOCOL_INSERT_GENERATE_MIN = 20;
    inline const std::int16_t VALID_SINGLE_PROTOCOL_INSERT_GENERATE = 20;
    inline const std::int16_t VALID_SINGLE_PROTOCOL_INSERT_GENERATE_COUNT = 21;
    inline const std::int16_t VALID_SINGLE_PROTOCOL_INSERT_GENERATE_LEVEL = 22;
    inline const std::int16_t VALID_SINGLE_PROTOCOL_INSERT_GENERATE_COUNT_LEVEL = 23;
    inline const std::int16_t VALID_SINGLE_PROTOCOL_INSERT_GENERATE_MAX = 23;
    */

    // ----- BATCH LEVELED LIST MULTI INSERTION PROTOCOLS -----

    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_MIN = 50;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_BASIC = 50;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_BASIC_COUNT = 51;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_BASIC_LEVEL = 52;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_BASIC_COUNT_LEVEL = 53;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_INSERT_MAX = 53;

    /*
    // generate new leveled list with insert items and insert alongside target each time target is found in leveled lists
    inline const std::int16_t VALID_MULTI_PROTOCOL_INSERT_GENERATE_MIN = 70;
    inline const std::int16_t VALID_MULTI_PROTOCOL_INSERT_GENERATE = 70;
    inline const std::int16_t VALID_MULTI_PROTOCOL_INSERT_GENERATE_COUNT = 71;
    inline const std::int16_t VALID_MULTI_PROTOCOL_INSERT_GENERATE_LEVEL = 72;
    inline const std::int16_t VALID_MULTI_PROTOCOL_INSERT_GENERATE_COUNT_LEVEL = 73;
    inline const std::int16_t VALID_MULTI_PROTOCOL_INSERT_GENERATE_MAX = 73;
    */

    // ----- BATCH LEVELED LIST SINGLE REMOVAL PROTOCOLS -----

    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_MIN = 100;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_BASIC = 100;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_BASIC_COUNT = 101;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_BASIC_LEVEL = 102;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_BASIC_COUNT_LEVEL = 103;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_NO_INSERT_REMOVE = 104;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_MAX = 104;

    /*
    // generate new leveled list with insert items, remove target once, and insert the generated leveled list once
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_GENERATE_MIN = 120;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_GENERATE = 120;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_GENERATE_COUNT = 121;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_GENERATE_LEVEL = 122;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_GENERATE_COUNT_LEVEL = 123;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_GENERATE_MAX = 123;
    */

    // ----- BATCH LEVELED LIST MULTI REMOVAL PROTOCOLS -----

    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_MIN = 150;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_BASIC = 150;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_BASIC_COUNT = 151;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_BASIC_LEVEL = 152;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_BASIC_COUNT_LEVEL = 153;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_NO_INSERT_REMOVE = 154;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_MAX = 154;

    // generate new leveled list with insert items, remove target each time it is found in leveled lists, and insert the generated leveled list each time target is found
    // leveled list will be generated, have items inserted, then entered into batch map with a protocol of 153
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_MIN = 170;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_BASIC = 170;
    /*inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_COUNT_ONE = 171;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_LEVEL_ONE = 172;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_COUNT_LEVEL_ONE = 173;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_USEALL = 174;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_USEALL_COUNT_ONE = 175;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_USEALL_LEVEL_ONE = 176;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_USEALL_COUNT_LEVEL_ONE = 177;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_ONCE = 178;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_ONCE_COUNT_ONE = 179;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_ONCE_LEVEL_ONE = 180;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_ONCE_COUNT_LEVEL_ONE = 181;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_ONCE_USEALL = 182;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_ONCE_USEALL_COUNT_ONE = 183;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_ONCE_USEALL_LEVEL_ONE = 184;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_ONCE_USEALL_COUNT_LEVEL_ONE = 185;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_EACH = 186;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_EACH_COUNT_ONE = 187;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_EACH_LEVEL_ONE = 188;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_EACH_COUNT_LEVEL_ONE = 189;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_EACH_USEALL = 190;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_EACH_USEALL_COUNT_ONE = 191;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_EACH_USEALL_LEVEL_ONE = 192;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_REINSERT_TARGET_EACH_USEALL_COUNT_LEVEL_ONE = 193;*/
    inline const std::uint16_t VALID_MULTI_PROTOCOL_SWAP_GENERATE_MAX = 170;
    
    // ----- SPECIFIC LEVELED LIST SINGLE INSERTION PROTOCOLS -----

    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_MIN = 200;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_BASIC = 200;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_MAX = 200;

    // ----- SPECIFIC LEVELED LIST SINGLE REMOVAL PROTOCOLS -----

    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MIN = 300;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_BASIC = 300;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MAX = 300;

    // ----- SPECIFIC LEVELED LIST MULTI REMOVAL PROTOCOLS -----

    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MIN = 350;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_BASIC = 350;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MAX = 350;

    // ----- BATCH OUTFIT SWAP PROTOCOL -----

    // remove, but replace with a generated leveled list
    inline const std::uint16_t VALID_MULTI_PROTOCOL_OUTFIT_SWAP_MIN = 450;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_OUTFIT_SWAP_BASIC = 450;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_OUTFIT_SWAP_MAX = 450;
    
    // ----- BATCH CONTAINER SINGLE INSERTION PROTOCOLS -----
    /*
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_INSERT_MIN = 500;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_INSERT_BASIC = 500;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_INSERT_BASIC_COUNT = 501;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_INSERT_MAX = 501;
    
    // ----- BATCH CONTAINER MULTI INSERTION PROTOCOLS -----

    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_INSERT_MIN = 550;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_INSERT_BASIC = 550;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_INSERT_BASIC_COUNT = 551;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_INSERT_MAX = 551;

    // ----- BATCH CONTAINER SINGLE REMOVAL PROTOCOLS -----
    
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_MIN = 600;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_BASIC = 600;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_BASIC_COUNT = 601;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_NO_INSERT = 602;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_REMOVE_MAX = 602;
    
    // ----- BATCH CONTAINER MULTI REMOVAL PROTOCOLS -----

    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_MIN = 650;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_BASIC = 650;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_BASIC_COUNT = 651;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_NO_INSERT = 652;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_REMOVE_MAX = 652;
    */
    // ----- BATCH CONTAINER SINGLE SWAP PROTOCOLS -----
    /*
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_SWAP_MIN = 700;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_SWAP_BASIC = 700;
    inline const std::uint16_t VALID_SINGLE_PROTOCOL_CONTAINER_SWAP_MAX = 700;
    */
    // ----- BATCH CONTAINER MULTI SWAP PROTOCOLS -----

    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_SWAP_MIN = 750;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_SWAP_BASIC = 750;
    inline const std::uint16_t VALID_MULTI_PROTOCOL_CONTAINER_SWAP_MAX = 750;

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
}

