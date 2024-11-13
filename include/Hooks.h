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
}