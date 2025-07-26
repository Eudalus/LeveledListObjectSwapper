#pragma once

#include <vector>
#include "GeneratedLeveledListInstruction.h"
#include "GenerateItemData.h"

template <typename T>
class GenerateCollection
{
public:
	std::vector<GenerateItemData> insertVector;
	RE::TESForm* targetForm{};
	T generatedLeveledList{}; // RE::TESLevItem*, RE::TESLevCharacter*, RE::TESLevSpell*
	uint8_t targetFormType{};
	GeneratedLeveledListInstruction instruction = GeneratedLeveledListInstruction::None;
};