#pragma once

#include "SimpleIni.h"
#include "ItemData.h"
#include "SmallerItemData.h"
#include "OutfitItemData.h"
#include "ContainerGenerateItemData.h"
#include "GeneratedLeveledListInstruction.h"
#include "boost_unordered.hpp"
#include "GenerateCollection.h"

class Utility
{
public:
    static void GetIniValue(const CSimpleIniA &iniFile, std::string &value,
                          const char *section, const char *key,
                          const std::string &backup = std::string());

    static unsigned int StringToUnInt(std::string value, unsigned int backup,
                                      unsigned int base = 10,
                                      bool debug = false);

    static float StringToFloat(std::string value, float backup,
                               bool debug = false);

    static bool AcquireFormData(const std::string &value, std::string &editor,
                                std::string &plugin, RE::FormID &formID);

    static bool CheckFormID(std::string &plugin, RE::FormID &formID);

    static bool CheckEditorID(const std::string &editor, RE::FormID &formID);

    static bool AcquireFormDataAndCheck(const std::string &value,
                                        std::string &editor,
                                        std::string &plugin,
                                        RE::FormID &formID);

    static RE::TESForm *AcquireForm(const std::string &value, std::string &editor, std::string &plugin, RE::FormID &formID);

    static bool AcquireRangeData(std::string &value, std::uint16_t &min,
                                 std::uint16_t &max, unsigned int backup,
                                 bool debug = false);

    static void SortLeveledListArrayByLevel(RE::SimpleArray<RE::LEVELED_OBJECT>& entries);

    static void SanitizeLeveledListArray(RE::SimpleArray<RE::LEVELED_OBJECT>& entries);

    /// <summary>
    /// Determines what leveled list can accept the form argument.
    /// </summary>
    /// <param name="form"></param>
    /// <returns>
    /// 0 if form is null or form is not an acceptable form type
    /// 1 if form can insert into a leveled item list
    /// 2 if form is armor, can insert into leveled list or outfit
    /// 3 if form is a leveled item list
    /// 4 if form can insert into a leveled npc list
    /// 5 if form is a leveled npc list
    /// 6 if form can insert into a leveled spell list
    /// 7 if form is a leveled spell list
    /// </returns>
    static int CheckFormType(const RE::TESForm* form);

    static bool CheckCompatibleLeveledListFormTypes(const std::uint8_t insert, const std::uint8_t target);

    static std::uint16_t ClampProtocol(const std::uint16_t value);

    static bool CompareLeveledListEntryLevel(const RE::LEVELED_OBJECT& a, const RE::LEVELED_OBJECT& b);

    static bool CheckCompatibleOutfitFormTypes(const std::uint8_t insert, const std::uint8_t target);

    static bool CheckCompatibleKeywordFormTypes(const std::uint8_t insert, const std::uint8_t target);

    static bool CheckCompatibleContainerFormTypes(const std::uint8_t insert, const std::uint8_t target);

    template<typename T> static RE::TESLevItem* CreateLeveledList(std::vector<T>& list);

    template<typename T> static T GenerateLeveledList(const RE::FormType& formType, std::vector<ContainerGenerateItemData>& list, const GeneratedLeveledListInstruction& instruction, const ItemData& targetItemData, size_t& targetReinsertCounter);

    template<typename T> static T GenerateLeveledList(GenerateCollection<T>& collection, std::vector<T>& pushBackList, size_t& targetReinsertCounter);

    template<typename T> static bool SetGeneratedLeveledListInstruction(GeneratedLeveledListInstruction& instruction, const T& modifier)
    {
        instruction = static_cast<GeneratedLeveledListInstruction>(instruction | static_cast<GeneratedLeveledListInstruction>(modifier));

        return true;
    }
};