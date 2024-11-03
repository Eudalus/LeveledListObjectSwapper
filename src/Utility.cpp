#include "Utility.h"
#include "Data.h"

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

float Utility::StringToFloat(std::string value, float backup, bool debug)
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
        if (CheckEditorID(editor, formID))
        {
            return true;
        }
        else if (CheckFormID(plugin, formID))
        {
            return true;
        }
    }

    return false;
}

RE::TESForm *Utility::AcquireForm(const std::string &value, std::string &editor, std::string &plugin, RE::FormID &formID)
{
    const auto dataHandler = RE::TESDataHandler::GetSingleton();

    RE::TESForm *form = nullptr;

    if (AcquireFormData(value, editor, plugin, formID))
    {
        if (g_mergeMapperInterface)
        {
            const auto newFormID = g_mergeMapperInterface->GetNewFormID(plugin.c_str(), formID);

            plugin = newFormID.first;
            formID = newFormID.second;

            form = RE::TESForm::LookupByID(dataHandler->LookupFormID(formID, plugin));

        }
        else
        {
            form = RE::TESForm::LookupByID(dataHandler->LookupFormID(formID, plugin));
        }

        if (!form)
        {
            form = RE::TESForm::LookupByEditorID(editor);
        }
    }

    return form;
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
                const auto maxValue = std::strtoul(maxString.c_str(), NULL, 10);

                // both values acquired
                min = std::clamp((int) std::min(minValue, maxValue), 1, 16384);
                max = std::clamp((int) std::max(minValue, maxValue), 1, 16384);

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
        else // only check for single value
        {
            try
            {
                // string to unsigned long
                min = std::clamp((int) std::strtoul(value.c_str(), NULL, 10), 1, 16384);
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

    // doesn't need clamp since controlled values
    min = backup;
    max = backup;

    return false;
}

void Utility::SanitizeLeveledListArray(RE::SimpleArray<RE::LEVELED_OBJECT> &entries)
{
    const auto size = entries.size();

    for (RE::SimpleArray<RE::LEVELED_OBJECT>::size_type i = 0; i < size; ++i)
    {
        entries[i].itemExtra = nullptr;
        entries[i].pad0C = 0;
    }
}

// refactor
void Utility::SortLeveledListArrayByLevel(RE::SimpleArray<RE::LEVELED_OBJECT> &entries)
{
    const auto size = entries.size();

    for (RE::SimpleArray<RE::LEVELED_OBJECT>::size_type i = 0; i < size; ++i)
    {
        for (RE::SimpleArray<RE::LEVELED_OBJECT>::size_type k = 0; (k + 1) < (size - i); ++k)
        {
            if (entries[k].level > entries[k + 1].level)
            {
                RE::LEVELED_OBJECT tempObject(entries[k + 1]);

                entries[k + 1] = entries[k];
                entries[k] = tempObject;

            }
        }
    }

    SanitizeLeveledListArray(entries);
}

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
int Utility::CheckFormType(const RE::TESForm *form)
{
    if (form)
    {
        const RE::FormType formType = form->GetFormType();

        switch (formType)
        {
        // ITEM
        case RE::FormType::Armor:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::Weapon:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::Misc:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::Ingredient:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::AlchemyItem:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::Book:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::Ammo:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::SoulGem:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::Scroll:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::LeveledItem:
            return Data::LEVELED_ITEM_FORM_TYPE;

        // NPC
        case RE::FormType::NPC:
            return Data::NPC_FORM_TYPE;
        case RE::FormType::LeveledNPC:
            return Data::LEVELED_NPC_FORM_TYPE;

        // SPELL
        case RE::FormType::Spell:
            return Data::SPELL_FORM_TYPE;
        case RE::FormType::LeveledSpell:
            return Data::LEVELED_SPELL_FORM_TYPE;

        // ITEM AGAIN
        case RE::FormType::KeyMaster:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::Light:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::Apparatus:
            return Data::ITEM_FORM_TYPE;
        case RE::FormType::Note:
            return Data::ITEM_FORM_TYPE;
        }
    }

    return Data::INVALID_FORM_TYPE;
}

bool Utility::CheckCompatibleFormTypes(const std::uint8_t insert, const std::uint8_t target)
{
    if ((insert == Data::INVALID_FORM_TYPE) || (target == Data::INVALID_FORM_TYPE))
    {
        return false;
    }
    else if ((insert == Data::ITEM_FORM_TYPE && ((target == Data::ITEM_FORM_TYPE) || (target == Data::LEVELED_ITEM_FORM_TYPE))) || (insert == Data::LEVELED_ITEM_FORM_TYPE && ((target == Data::ITEM_FORM_TYPE) || (target == Data::LEVELED_ITEM_FORM_TYPE))))
    {
        return true;
    }
    else if ((insert == Data::NPC_FORM_TYPE && ((target == Data::NPC_FORM_TYPE) || (target == Data::LEVELED_NPC_FORM_TYPE))) || (insert == Data::LEVELED_NPC_FORM_TYPE && ((target == Data::NPC_FORM_TYPE) || (target == Data::LEVELED_NPC_FORM_TYPE))))
    {
        return true;
    }
    else if ((insert == Data::SPELL_FORM_TYPE && ((target == Data::SPELL_FORM_TYPE) || (target == Data::LEVELED_SPELL_FORM_TYPE))) || (insert == Data::LEVELED_SPELL_FORM_TYPE && ((target == Data::SPELL_FORM_TYPE) || (target == Data::LEVELED_SPELL_FORM_TYPE))))
    {
        return true;
    }

    return false;
}

std::uint16_t Utility::ClampProtocol(const std::uint16_t value)
{
    if (((value >= Data::VALID_SINGLE_PROTOCOL_INSERT_MIN) && (value <= Data::VALID_SINGLE_PROTOCOL_INSERT_MAX)) ||
        ((value >= Data::VALID_MULTI_PROTOCOL_INSERT_MIN) && (value <= Data::VALID_MULTI_PROTOCOL_INSERT_MAX)) ||
        ((value >= Data::VALID_SINGLE_PROTOCOL_REMOVE_MIN) && (value <= Data::VALID_SINGLE_PROTOCOL_REMOVE_MAX)) ||
        ((value >= Data::VALID_MULTI_PROTOCOL_REMOVE_MIN) && (value <= Data::VALID_MULTI_PROTOCOL_REMOVE_MAX)) ||
        ((value >= Data::VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_MIN) && (value <= Data::VALID_SINGLE_PROTOCOL_INSERT_TARGET_LEVELED_LIST_MAX)) ||
        ((value >= Data::VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MIN) && (value <= Data::VALID_SINGLE_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MAX)) ||
        ((value >= Data::VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MIN) && (value <= Data::VALID_MULTI_PROTOCOL_REMOVE_TARGET_LEVELED_LIST_MAX)))
    {
        return value;
    }
    else
    {
        return Data::DEFAULT_PROTOCAL_VALUE;
    }
}