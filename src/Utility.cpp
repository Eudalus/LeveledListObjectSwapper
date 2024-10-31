#include "Utility.h"

namespace logger = SKSE::log;

void Utility::GetIniValue(const CSimpleIniA &iniFile, std::string &value, const char *section, const char *key, const std::string &backup)
{
    value = iniFile.GetValue(section, key, backup.empty() ? value.c_str() : backup.c_str());
}

unsigned int Utility::StringToUnInt(std::string value, unsigned int backup, unsigned int base, bool debug)
{
    try
    {
        return std::stoull(value);

    }
    catch (const std::invalid_argument &ia)
    {
        if (debug)
        {
            logger::warn("Invalid value: {} --- returning backup: {}",
                         ia.what(), backup);
        }
    }

    return backup;
}

float Utility::StringToFloat(std::string value, float backup, bool debug = false)
{
    try
    {
        return std::stof(value);
    }
    catch (const std::invalid_argument &ia)
    {
        if (debug)
        {
            logger::warn("Invalid value: {} --- returning backup: {}",
                         ia.what(), backup);
        }
    }

    return backup;
}

bool Utility::AcquireFormData(const std::string &value, std::string &editor, std::string &plugin, RE::FormID &formID)
{
    if (!value.empty())
    {
        const int position = value.find('~');

        if ((position != std::string::npos) && ((position + 1) < value.size())) // form id
        {
            std::string targetValue = value.substr(0, position);
            std::string pluginValue = value.substr(position + 1);

            try
            {
                formID = std::strtoul(targetValue.c_str(), NULL, 16);
                plugin = pluginValue.c_str();

                return true;
            }
            catch (const std::invalid_argument &ia)
            {
                logger::warn("Invalid form: {} --- {}", ia.what(), targetValue);
                return false;
            }
        }
        else // editor id
        {
            editor = value.c_str();

            return true;
        }
    }
    return false;
}

bool Utility::CheckFormID(std::string &plugin, RE::FormID &formID)
{
    const auto dataHandler = RE::TESDataHandler::GetSingleton();

    if (g_mergeMapperInterface)
    {
        const auto newFormID =
            g_mergeMapperInterface->GetNewFormID(plugin.c_str(), formID);

        plugin = newFormID.first;
        formID = newFormID.second;

        const auto item = dataHandler->LookupFormID(formID, plugin);

        if (item)
        {
            formID = item;
            return true;
        }
    }
    else
    {
        const auto item = dataHandler->LookupFormID(formID, plugin);

        if (item)
        {
            formID = item;
            return true;
        }
    }

    return false;
}

bool Utility::CheckEditorID(const std::string &editor,
                                 RE::FormID &formID)
{
    if (editor.empty())
    {
        return false;
    }

    const auto item = RE::TESForm::LookupByEditorID(editor);

    if (item)
    {
        formID = item->formID;

        return true;
    }

    return false;
}

bool Utility::AcquireFormDataAndCheck(const std::string &value,
                                    std::string &editor, std::string &plugin,
                                    RE::FormID &formID)
{
    if (AcquireFormData(value, editor, plugin, formID))
    {
        if (CheckEditorID(editor, formID)) {
            return true;
        } else if (CheckFormID(plugin, formID)) {
            return true;
        }
    }

    return false;
}

bool Utility::AcquireRangeData(std::string &value, std::uint16_t &min,
                             std::uint16_t &max, unsigned int backup,
                             bool debug)
{
    if (!value.empty())
    {
        const int position = value.find('~');

        if ((position != std::string::npos) && ((position + 1) < value.size())) // check for min and max value
        {
            std::string minString = value.substr(0, position);
            std::string maxString = value.substr(position + 1);

            try
            {
                // string to unsigned long
                const auto minValue = std::strtoul(minString.c_str(), NULL, 10);
                const auto maxValue = std::strtoul(minString.c_str(), NULL, 10);

                // both values acquired
                min = minValue;
                max = maxValue;

                return true;
            } catch (const std::invalid_argument &ia)
            {
                if (debug)
                {
                    logger::warn("Invalid value: {} --- {}", ia.what(), value);
                }
            }
        }
        else // only check for single value
        {
            try
            {
                // string to unsigned long
                min = std::strtoul(value.c_str(), NULL, 10);
                max = min;

                return true;
            }
            catch (const std::invalid_argument &ia)
            {
                if (debug)
                {
                    logger::warn("Invalid value: {} --- {}", ia.what(), value);
                }
            }
        }
    }

    min = backup;
    max = backup;

    return false;
}

bool Utility::CheckForLeveledList()
{
    return false;
}