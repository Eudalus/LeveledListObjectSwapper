#include "Hooks.h"
#include "UpkeepManager.h"
namespace logger = SKSE::log;

/*
namespace InitHooks
{

	static std::size_t outfitCounter = 0;
	static std::size_t itemCounter = 0;
	static std::size_t npcCounter = 0;
	static std::size_t spellCounter = 0;
	static size_t objectCounter = 0;
	static size_t realObjectCounter = 0;

	// ----- OUTFIT
	void OutfitInitItemImplHook::InitItemImpl(RE::BGSOutfit* outfit)
	{
		_InitItemImpl(outfit);

		if (outfit->formID == 0x09D5DF)
		{
			for (int i = 0; i < outfit->outfitItems.size(); ++i)
			{
				if (outfit->outfitItems[i]->formID == 0x03452E)
				{
					logger::info("CHANGING OUTFIT DATA IN BGSOUTFIT -> INIT ITEM IMPL HOOK");
					logger::info("OUTFIT FORM FLAGS: {}", outfit->formFlags);
					logger::info("OUTFIT IN GAME FORM FLAGS: {}", static_cast<size_t>(outfit->inGameFormFlags.get()));
					outfit->outfitItems[i] = RE::TESForm::LookupByID(0x01396B);
				}
			}
		}

		//outfitCounter += 1;

		//const auto lockpick = RE::TESForm::LookupByID(0xA);

		//logger::info("{:*^6} --- OUTFIT SIZE: {} --- FORM ID: {}", outfitCounter, (outfit ? outfit->outfitItems.size() : 0), std::format("{:x}",(lockpick ? lockpick->formID : 0)));
		
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

	void TESObjectREFRInitItemImplHook::InitItemImpl(RE::TESObjectREFR* object)
	{
		_InitItemImpl(object);


		
	}

	void TESObjectREFRInitItemImplHook::Hook()
	{
		// 0x13 address of InitItemImpl function
        _InitItemImpl = REL::Relocation<uintptr_t>(RE::VTABLE_TESObjectREFR[0]).write_vfunc(0x13, InitItemImpl);
	}
}
*/
namespace LoadHooks
{
	/*
	static std::size_t objectLoadCounter = 0;
	static std::size_t objectRealLoadCounter = 0;
	static size_t actorLoadCounter = 0;
	static size_t actorReadLoadCounter =0;
	*/
	static size_t characterLoadCounter =0;
	static size_t characterRealLoadCounter=0;
	/*
	// ----- TESObjectREFR Load3D hook
	RE::NiAVObject* TESObjectREFRLoad3DHook::Load3D(RE::TESObjectREFR* object, bool a_backgroundLoading)
	{
		auto value = _Load3D(object, a_backgroundLoading);
		++objectLoadCounter;
		if (object)
		{
			++objectRealLoadCounter;
			logger::info("{:*^6} --- {:*^6} --- TESObjectREFR -> LOAD 3D HOOK --- OBJECT TYPE: {} --- OBJECT FORM ID: {} --- OBJECT NAME: {}",objectLoadCounter, objectRealLoadCounter, static_cast<size_t>(object->GetFormType()) ,std::format("{:x}",object->formID), object->GetDisplayFullName());
		}

		return value;
	}

	void TESObjectREFRLoad3DHook::Hook()
	{
		// 0x6A address of Load3D
		_Load3D = REL::Relocation<uintptr_t>(RE::VTABLE_TESObjectREFR[0]).write_vfunc(0x6A, Load3D);
	}

	// ----- Actor Load3D hook
	RE::NiAVObject* ActorLoad3DHook::Load3D(RE::Actor* object, bool a_backgroundLoading)
	{
		auto value = _Load3D(object, a_backgroundLoading);

		++actorLoadCounter;
		logger::info("{}", actorLoadCounter);
		if (object)
		{
			++actorReadLoadCounter;
			logger::info("{:*^6} --- {:*^6} --- Actor -> LOAD 3D HOOK --- OBJECT TYPE: {} --- OBJECT FORM ID: {} --- OBJECT NAME: {}",actorLoadCounter, actorReadLoadCounter, static_cast<size_t>(object->GetFormType()) ,std::format("{:x}",object->formID), object->GetDisplayFullName());
		}

		return value;
	}

	void ActorLoad3DHook::Hook()
	{
		// 0x6A address of Load3D
		_Load3D = REL::Relocation<uintptr_t>(RE::VTABLE_Actor[0]).write_vfunc(0x6A, Load3D);
	}
	*/
	// ----- Character Load3D hook
	RE::NiAVObject* CharacterLoad3DHook::Load3D(RE::Character* object, bool a_backgroundLoading)
	{
		auto value = _Load3D(object, a_backgroundLoading); 

		++characterLoadCounter;
		//logger::info("{}", actorLoadCounter);
		//if (object && UpkeepManager::GetSingleton()->allowOutfitCheck && UpkeepManager::GetSingleton()->CheckOutfit(object))
		if (object)
		{
			
			//object->HasOutfitItems() // needs testing
			++characterRealLoadCounter;
			logger::info("{:*^6} --- {:*^6} --- Character -> LOAD 3D HOOK --- OBJECT TYPE: {} --- OBJECT FORM ID: {} --- OBJECT NAME: {}",characterLoadCounter, characterRealLoadCounter, static_cast<size_t>(object->GetFormType()) ,std::format("{:x}",object->formID), object->GetDisplayFullName());
			UpkeepManager::GetSingleton()->CheckOutfit(object);
			//UpkeepManager::GetSingleton()->allowOutfitCheck = false;
			
			/*
			object->RemoveOutfitItems(object->GetActorBase()->defaultOutfit);
			object->AddWornOutfit(object->GetActorBase()->defaultOutfit, false);

			object->RemoveOutfitItems(object->GetActorBase()->sleepOutfit);
			object->AddWornOutfit(object->GetActorBase()->sleepOutfit, false);

			object->InitInventoryIfRequired(false);
			*/

			//UpkeepManager::GetSingleton()->allowOutfitCheck = true;
		}

		return value;
	}

	void CharacterLoad3DHook::Hook()
	{
		// 0x6A address of Load3D
		_Load3D = REL::Relocation<uintptr_t>(RE::VTABLE_Character[0]).write_vfunc(0x6A, Load3D);
	}
}