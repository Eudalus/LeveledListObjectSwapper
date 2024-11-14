#pragma once


namespace stl
{
    using namespace SKSE::stl;

    template <class T>
    void write_thunk_call(std::uintptr_t a_src)
	{
        auto& trampoline = SKSE::GetTrampoline();
        SKSE::AllocTrampoline(14);
        T::func = trampoline.write_call<5>(a_src, T::thunk);
    }

	template <class T>
    void write_thunk_branch(std::uintptr_t a_src)
	{
        auto& trampoline = SKSE::GetTrampoline();
        SKSE::AllocTrampoline(14);
        T::func = trampoline.write_branch<5>(a_src, T::thunk);
    }
}


namespace InitHooks
{
	// ----- OUTFIT
	class OutfitInitItemImplHook
	{
	public:

		static void InitItemImpl(RE::BGSOutfit* outfit);

		static inline REL::Relocation<decltype(InitItemImpl)> _InitItemImpl;

		static void Hook();
	};

	// ----- ITEM LEVELED LIST
	class LeveledItemInitItemImplHook
	{
	public:
		static void InitItemImpl(RE::TESLevItem* itemList);

		static inline REL::Relocation<decltype(InitItemImpl)> _InitItemImpl;

		static void Hook();
	};

	// ----- NPC LEVELED LIST
	class LeveledNPCInitItemImplHook
	{
	public:
		static void InitItemImpl(RE::TESLevCharacter* npcList);

		static inline REL::Relocation<decltype(InitItemImpl)> _InitItemImpl;

		static void Hook();
	};

	// ----- SPELL LEVELED LIST
	class LeveledSpellInitItemImplHook
	{
	public:
		static void InitItemImpl(RE::TESLevSpell* spellList);

		static inline REL::Relocation<decltype(InitItemImpl)> _InitItemImpl;

		static void Hook();
	};

	// ----- TESObjectREFR
	class TESObjectREFRInitItemImplHook
	{
	public:
		static void InitItemImpl(RE::TESObjectREFR* object);

		static inline REL::Relocation<decltype(InitItemImpl)> _InitItemImpl;

		static void Hook();
	};
}

namespace LoadHooks
{
	// ----- TESObjectREFR Load3D hook
	class TESObjectREFRLoad3DHook
	{
	public:
		static RE::NiAVObject* Load3D(RE::TESObjectREFR* object, bool a_backgroundLoading);

		static inline REL::Relocation<decltype(Load3D)> _Load3D;

		static void Hook();
	};

	// ----- Actor Load3D hook
	class ActorLoad3DHook
	{
	public:
		static RE::NiAVObject* Load3D(RE::Actor* object, bool a_backgroundLoading);

		static inline REL::Relocation<decltype(Load3D)> _Load3D;

		static void Hook();
	};

	// ----- Character Load3D hook
	class CharacterLoad3DHook
	{
	public:
		static RE::NiAVObject* Load3D(RE::Character* object, bool a_backgroundLoading);

		static inline REL::Relocation<decltype(Load3D)> _Load3D;

		static void Hook();
	};
}