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

    static bool AcquireRangeData(std::string &value, std::uint16_t &min,
                                 std::uint16_t &max, unsigned int backup,
                                 bool debug = false);

    static bool CheckForLeveledList();

    static void SortLeveledListArrayByLevel(RE::SimpleArray<RE::LEVELED_OBJECT>& entries);

    static void SanitizeLeveledListArray(RE::SimpleArray<RE::LEVELED_OBJECT>& entries);
};