#pragma once

#include "SimpleIni.h"

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
    /// 2 if form is a leveled item list
    /// 3 if form can insert into a leveled npc list
    /// 4 if form is a leveled npc list
    /// 5 if form can insert into a leveled spell list
    /// 6 if form is a leveled spell list
    /// </returns>
    static int CheckFormType(const RE::TESForm* form);

    static bool CheckCompatibleFormTypes(const std::uint8_t insert, const std::uint8_t target);

    static std::uint16_t ClampProtocol(const std::uint16_t value);
};