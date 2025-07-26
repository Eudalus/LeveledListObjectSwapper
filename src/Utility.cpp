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
        //entries[i].pad0C = 0;
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

        //entries[i].itemExtra = nullptr;
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
/// 2 if form is armor, can insert into leveled list or outfit
/// 3 if form is a leveled item list
/// 4 if form can insert into a leveled npc list
/// 5 if form is a leveled npc list
/// 6 if form can insert into a leveled spell list
/// 7 if form is a leveled spell list
/// </returns>
int Utility::CheckFormType(const RE::TESForm *form)
{
    if (form)
    {
        const RE::FormType formType = form->GetFormType();

        switch (formType)
        {
        // ARMOR
        case RE::FormType::Armor:
            return Data::ARMOR_FORM_TYPE;

        // ITEM
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
        // KEYWORD
        case RE::FormType::Keyword:
            return Data::KEYWORD_FORM_TYPE;

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

bool Utility::CheckCompatibleLeveledListFormTypes(const std::uint8_t insert, const std::uint8_t target)
{
    /* // unnecessary check?
    if ((insert == Data::INVALID_FORM_TYPE) || (target == Data::INVALID_FORM_TYPE))
    {
        return false;
    }*/
    //else if ((insert == Data::ITEM_FORM_TYPE && ((target == Data::ITEM_FORM_TYPE) || (target == Data::LEVELED_ITEM_FORM_TYPE))) || (insert == Data::LEVELED_ITEM_FORM_TYPE && ((target == Data::ITEM_FORM_TYPE) || (target == Data::LEVELED_ITEM_FORM_TYPE))))
    if(((insert >= Data::ITEM_FORM_TYPE) && (insert <= Data::LEVELED_ITEM_FORM_TYPE)) && ((target >= Data::ITEM_FORM_TYPE) && (target <= Data::LEVELED_ITEM_FORM_TYPE)))
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

bool Utility::CompareLeveledListEntryLevel(const RE::LEVELED_OBJECT& a, const RE::LEVELED_OBJECT& b)
{
    return a.level < b.level;
}

bool Utility::CheckCompatibleOutfitFormTypes(const std::uint8_t insert, const std::uint8_t target)
{
    /*
    if (((target >= Data::ARMOR_FORM_TYPE) && (target <= Data::LEVELED_ITEM_FORM_TYPE)) && ((insert >= Data::ARMOR_FORM_TYPE) && (insert <= Data::LEVELED_ITEM_FORM_TYPE)))
    {
        return true;
    }
    */

    return (((insert >= Data::ARMOR_FORM_TYPE) && (insert <= Data::LEVELED_ITEM_FORM_TYPE)) && ((target >= Data::ARMOR_FORM_TYPE) && (target <= Data::LEVELED_ITEM_FORM_TYPE)));
}

bool Utility::CheckCompatibleKeywordFormTypes(const std::uint8_t insert, const std::uint8_t target)
{
    /*
    if ((target == Data::KEYWORD_FORM_TYPE) && ((insert >= Data::ITEM_FORM_TYPE) && (insert <= Data::LEVELED_SPELL_FORM_TYPE)))
    {
        return true;
    }
    */

    return ((target == Data::KEYWORD_FORM_TYPE) && ((insert >= Data::ITEM_FORM_TYPE) && (insert <= Data::LEVELED_SPELL_FORM_TYPE)));
}

bool Utility::CheckCompatibleContainerFormTypes(const std::uint8_t insert, const std::uint8_t target)
{
    return (((insert >= Data::ITEM_FORM_TYPE) && (insert <= Data::LEVELED_ITEM_FORM_TYPE)) && ((target >= Data::ITEM_FORM_TYPE) && (target <= Data::LEVELED_ITEM_FORM_TYPE)));
}

template<typename T> RE::TESLevItem* Utility::CreateLeveledList(std::vector<T>& list)
{
    if (list.empty())
    {
        return nullptr;
    }

    auto& leveledItemLists = RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESLevItem>();

    RE::TESLevItem* value = nullptr;

    const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::TESLevItem>();

    uint8_t size;

    if (factory)
    {
        value = factory->Create();

        if (value)
        {
            size = std::min(list.size(), Data::MAX_ENTRY_SIZE);

            value->entries.resize(size);

            RE::SimpleArray<RE::LEVELED_OBJECT>& entries = value->entries;

            for (uint8_t i = 0; i < size; ++i)
            {
                entries[i].form = list[i].insertForm;
                entries[i].count = (rand() % (list[i].maxCount - list[i].minCount + 1)) + list[i].minCount;
                entries[i].level = (rand() % (list[i].maxLevel - list[i].minLevel + 1)) + list[i].minLevel;
                entries[i].itemExtra = nullptr;
            }

            // sort, don't need to sanitize since data insert loop handles it
            std::sort(entries.begin(), entries.end(), Utility::CompareLeveledListEntryLevel);

            value->numEntries = size;
            value->chanceNone = 0;
            value->llFlags = Data::GENERATED_LEVELED_LIST_FLAGS;

            leveledItemLists.push_back(value);
        }
    }

    return value;
}

// necessary signatures for the linker
template RE::TESLevItem* Utility::CreateLeveledList(std::vector<ContainerGenerateItemData>& list);
template RE::TESLevItem* Utility::CreateLeveledList(std::vector<OutfitItemData>& list);

template<typename T> static T Utility::GenerateLeveledList(const RE::FormType& formType, std::vector<ContainerGenerateItemData>& list, const GeneratedLeveledListInstruction& instruction, const ItemData& targetItemData, size_t& targetReinsertCounter)
{
    if (list.empty())
    {
        return T{};
    }

    //auto& leveledItemLists = RE::TESDataHandler::GetSingleton()->GetFormArray<std::remove_pointer_t<T>>(); // move to PushGeneratedLeveledList

    T value{}; // nullptr or default initialized

    const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<std::remove_pointer_t<T>>();

    uint8_t size;
    uint8_t numberOfInsertData;
    uint8_t entriesIndex = 0;
    bool reinsertTargetFirst = false;

    if (factory)
    {
        value = factory->Create();

        if (value)
        {
            if (targetItemData.targetFormType != Data::KEYWORD_FORM_TYPE)
            {
                if (((instruction & GeneratedLeveledListInstruction::ReinsertTargetOnce) == GeneratedLeveledListInstruction::ReinsertTargetOnce))
                {
                    size = std::min(list.size() + 1, Data::MAX_ENTRY_SIZE);
                    numberOfInsertData = std::min(list.size() + 1, Data::MAX_ENTRY_SIZE);

                    value->entries.resize(size);

                    reinsertTargetFirst = true;
                }
                else if (((instruction & GeneratedLeveledListInstruction::ReinsertTargetEachTime) == GeneratedLeveledListInstruction::ReinsertTargetEachTime))
                {
                    //size = std::min((list.size() * 2) + 1, Data::MAX_ENTRY_SIZE);
                    size = std::min((list.size() * 2), Data::MAX_ENTRY_SIZE);
                    numberOfInsertData = std::min(list.size() + 1, Data::MAX_ENTRY_SIZE);

                    value->entries.resize(size);

                    reinsertTargetFirst = true;
                }
                else
                {
                    size = std::min(list.size(), Data::MAX_ENTRY_SIZE);
                    numberOfInsertData = std::min(list.size(), Data::MAX_ENTRY_SIZE);

                    value->entries.resize(size);
                }
            }
            else
            {
                size = std::min(list.size(), Data::MAX_ENTRY_SIZE);
                numberOfInsertData = std::min(list.size(), Data::MAX_ENTRY_SIZE);

                value->entries.resize(size);
            } 

            RE::SimpleArray<RE::LEVELED_OBJECT>& entries = value->entries;

            if(reinsertTargetFirst)
            {
                // ensure target item enters the generated leveled list
                // entriesIndex = 0
                entries[entriesIndex].form = targetItemData.targetForm;
                entries[entriesIndex].count = 1;
                entries[entriesIndex].level = 1;
                entries[entriesIndex].itemExtra = nullptr;

                entriesIndex = 1;

                ++targetReinsertCounter;
            }

            // insert items
            for (size_t listIndex = 0; entriesIndex < numberOfInsertData; ++entriesIndex, ++listIndex)
            {
                entries[entriesIndex].form = list[listIndex].insertForm;
                entries[entriesIndex].count = (rand() % (list[listIndex].maxCount - list[listIndex].minCount + 1)) + list[listIndex].minCount;
                entries[entriesIndex].level = (rand() % (list[listIndex].maxLevel - list[listIndex].minLevel + 1)) + list[listIndex].minLevel;
                entries[entriesIndex].itemExtra = nullptr;
            }

            // try to ensure target form is in leveled list equal to number of insert items
            // size should only be greater than entriesIndex if GeneratedLeveledListInstruction::ReinsertTargetEachTime was set and there's still room for more items (entriesIndex < 255)
            for (; entriesIndex < size; ++entriesIndex)
            {
                entries[entriesIndex].form = targetItemData.targetForm;
                entries[entriesIndex].count = 1;
                entries[entriesIndex].level = 1;
                entries[entriesIndex].itemExtra = nullptr;

                ++targetReinsertCounter;
            }

            value->numEntries = static_cast<uint8_t>(size);
            value->chanceNone = 0;
            value->llFlags = static_cast<RE::TESLeveledList::Flag>(Data::GENERATED_LEVELED_LIST_FLAGS | static_cast<RE::TESLeveledList::Flag>(instruction & GeneratedLeveledListInstruction::AddUseAllFlag));

            // sort, don't need to sanitize since data insert loop handles it
            std::sort(entries.begin(), entries.end(), Utility::CompareLeveledListEntryLevel);

            //leveledItemLists.push_back(value); // move to PushGeneratedLeveledList
        }
    }

    return value;
}

// necessary signatures for the linker
template RE::TESLevItem* Utility::GenerateLeveledList<RE::TESLevItem*>(const RE::FormType& formType, std::vector<ContainerGenerateItemData>& list, const GeneratedLeveledListInstruction& instruction, const ItemData& targetItemData, size_t& targetReinsertCounter);
template RE::TESLevCharacter* Utility::GenerateLeveledList<RE::TESLevCharacter*>(const RE::FormType& formType, std::vector<ContainerGenerateItemData>& list, const GeneratedLeveledListInstruction& instruction, const ItemData& targetItemData, size_t& targetReinsertCounter);
template RE::TESLevSpell* Utility::GenerateLeveledList<RE::TESLevSpell*>(const RE::FormType& formType, std::vector<ContainerGenerateItemData>& list, const GeneratedLeveledListInstruction& instruction, const ItemData& targetItemData, size_t& targetReinsertCounter);

template<typename T> T Utility::GenerateLeveledList(GenerateCollection<T>& collection, size_t& targetReinsertCounter)
{
    T value{}; // nullptr or default initialized

    if (collection.insertVector.empty())
    {
        return value;
    }

    const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<std::remove_pointer_t<T>>();

    if (!factory)
    {
        return value;
    }

    value = factory->Create();

    if (!value)
    {
        return value;
    }

    uint8_t size;
    uint8_t numberOfInsertData;
    uint8_t entriesIndex = 0;
    bool reinsertTargetFirst = false;

    if (collection.targetFormType != Data::KEYWORD_FORM_TYPE) // can only reinsert non-keyword types
    {
        if (((collection.instruction & GeneratedLeveledListInstruction::ReinsertTargetOnce) == GeneratedLeveledListInstruction::ReinsertTargetOnce))
        {
            size = std::min(collection.insertVector.size() + 1, Data::MAX_ENTRY_SIZE);
            numberOfInsertData = std::min(collection.insertVector.size() + 1, Data::MAX_ENTRY_SIZE);

            value->entries.resize(size);

            reinsertTargetFirst = true;
        }
        else if (((collection.instruction & GeneratedLeveledListInstruction::ReinsertTargetEachTime) == GeneratedLeveledListInstruction::ReinsertTargetEachTime))
        {
            //size = std::min((list.size() * 2) + 1, Data::MAX_ENTRY_SIZE);
            size = std::min((collection.insertVector.size() * 2), Data::MAX_ENTRY_SIZE);
            numberOfInsertData = std::min(collection.insertVector.size() + 1, Data::MAX_ENTRY_SIZE);

            value->entries.resize(size);

            reinsertTargetFirst = true;
        }
        else
        {
            size = std::min(collection.insertVector.size(), Data::MAX_ENTRY_SIZE);
            numberOfInsertData = std::min(collection.insertVector.size(), Data::MAX_ENTRY_SIZE);

            value->entries.resize(size);
        }
    }
    else // cannot reinsert keyword types
    {
        size = std::min(collection.insertVector.size(), Data::MAX_ENTRY_SIZE);
        numberOfInsertData = std::min(collection.insertVector.size(), Data::MAX_ENTRY_SIZE);

        value->entries.resize(size);
    } 

    RE::SimpleArray<RE::LEVELED_OBJECT>& entries = value->entries;

    if(reinsertTargetFirst)
    {
        // ensure target item enters the generated leveled list
        // entriesIndex = 0
        entries[entriesIndex].form = collection.targetForm;
        entries[entriesIndex].count = 1;
        entries[entriesIndex].level = 1;
        entries[entriesIndex].itemExtra = nullptr;

        entriesIndex = 1;

        ++targetReinsertCounter;
    }

    std::vector<GenerateItemData>& list = collection.insertVector;

    // insert items
    for (size_t listIndex = 0; entriesIndex < numberOfInsertData; ++entriesIndex, ++listIndex)
    {
        entries[entriesIndex].form = list[listIndex].insertForm;
        entries[entriesIndex].count = list[listIndex].count; //(rand() % (list[listIndex].maxCount - list[listIndex].minCount + 1)) + list[listIndex].minCount;
        entries[entriesIndex].level = list[listIndex].level; //(rand() % (list[listIndex].maxLevel - list[listIndex].minLevel + 1)) + list[listIndex].minLevel;
        entries[entriesIndex].itemExtra = nullptr;
    }

    // try to ensure target form is in leveled list equal to number of insert items
    // size should only be greater than entriesIndex if GeneratedLeveledListInstruction::ReinsertTargetEachTime was set and there's still room for more items (entriesIndex < 255)
    for (; entriesIndex < size; ++entriesIndex)
    {
        entries[entriesIndex].form = collection.targetForm;
        entries[entriesIndex].count = 1;
        entries[entriesIndex].level = 1;
        entries[entriesIndex].itemExtra = nullptr;

        ++targetReinsertCounter;
    }

    value->numEntries = static_cast<uint8_t>(size);
    value->chanceNone = 0;
    value->llFlags = static_cast<RE::TESLeveledList::Flag>(Data::GENERATED_LEVELED_LIST_FLAGS | static_cast<RE::TESLeveledList::Flag>(collection.instruction & GeneratedLeveledListInstruction::AddUseAllFlag));

    // sort, don't need to sanitize since data insert loop handles it
    std::sort(entries.begin(), entries.end(), Utility::CompareLeveledListEntryLevel);

    return value;
}

// necessary signatures for the linker
template RE::TESLevItem* Utility::GenerateLeveledList<RE::TESLevItem*>(GenerateCollection<RE::TESLevItem*>& collection, size_t& targetReinsertCounter);
template RE::TESLevCharacter* Utility::GenerateLeveledList<RE::TESLevCharacter*>(GenerateCollection<RE::TESLevCharacter*>& collection, size_t& targetReinsertCounter);
template RE::TESLevSpell* Utility::GenerateLeveledList<RE::TESLevSpell*>(GenerateCollection<RE::TESLevSpell*>& collection, size_t& targetReinsertCounter);
