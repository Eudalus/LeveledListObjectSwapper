#include "Hooks.h"

/*
namespace logger = SKSE::log;

namespace InitHooks
{

	static std::size_t outfitCounter = 0;
	static std::size_t itemCounter = 0;
	static std::size_t npcCounter = 0;
	static std::size_t spellCounter = 0;

	// ----- OUTFIT
	void OutfitInitItemImplHook::InitItemImpl(RE::BGSOutfit* outfit)
	{
		_InitItemImpl(outfit);

		outfitCounter += 1;

		const auto lockpick = RE::TESForm::LookupByID(0xA);

		logger::info("{:*^6} --- OUTFIT SIZE: {} --- FORM ID: {}", outfitCounter, (outfit ? outfit->outfitItems.size() : 0), std::format("{:x}",(lockpick ? lockpick->formID : 0)));
		
	}

	void OutfitInitItemImplHook::Hook()
	{
		// 0x13 address of InitItemImpl function
        _InitItemImpl = REL::Relocation<uintptr_t>(RE::VTABLE_BGSOutfit[0]).write_vfunc(0x13, InitItemImpl);
	}

	// ----- ITEM LEVELED LIST
	void LeveledItemInitItemImplHook::InitItemImpl(RE::TESLevItem* itemList)
	{
		

		itemCounter += 1;

		const auto lockpick = RE::TESForm::LookupByID(0xA);

		logger::info("{:*^6} --- LEVELED ITEM NUMBER OF ENTRIES: {} --- FORM ID: {}", itemCounter, itemList->numEntries, std::format("{:x}",(itemList ? itemList->formID : 0)));

		_InitItemImpl(itemList);
	}

	void LeveledItemInitItemImplHook::Hook()
	{
		// 0x13 address of InitItemImpl function
        _InitItemImpl = REL::Relocation<uintptr_t>(RE::VTABLE_TESLevItem[0]).write_vfunc(0x13, InitItemImpl);
	}

	// ----- NPC LEVELED LIST
	void LeveledNPCInitItemImplHook::InitItemImpl(RE::TESLevCharacter* npcList)
	{
		_InitItemImpl(npcList);

		npcCounter += 1;

		const auto lockpick = RE::TESForm::LookupByID(0xA);

		logger::info("{:*^6} --- LEVELED ITEM NUMBER OF ENTRIES: {} --- FORM ID: {}", npcCounter, (npcList ? npcList->numEntries : 0), std::format("{:x}",(lockpick ? lockpick->formID : 0)));
	}

	void LeveledNPCInitItemImplHook::Hook()
	{
		// 0x13 address of InitItemImpl function
        _InitItemImpl = REL::Relocation<uintptr_t>(RE::VTABLE_TESLevCharacter[0]).write_vfunc(0x13, InitItemImpl);
	}

	// ----- SPELL LEVELED LIST
	void LeveledSpellInitItemImplHook::InitItemImpl(RE::TESLevSpell* spellList)
	{
		_InitItemImpl(spellList);

		spellCounter += 1;

		const auto lockpick = RE::TESForm::LookupByID(0xA);

		logger::info("{:*^6} --- LEVELED ITEM NUMBER OF ENTRIES: {} --- FORM ID: {}", spellCounter, spellList->numEntries, std::format("{:x}",(lockpick ? lockpick->formID : 0)));
	}

	void LeveledSpellInitItemImplHook::Hook()
	{
		// 0x13 address of InitItemImpl function
        _InitItemImpl = REL::Relocation<uintptr_t>(RE::VTABLE_TESLevSpell[0]).write_vfunc(0x13, InitItemImpl);
	}
}
*/