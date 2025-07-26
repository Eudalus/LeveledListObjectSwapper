#pragma once

#include <vector>
#include "GeneratedLeveledListInstruction.h"
#include "GenerateItemData.h"

class GenerateCollection
{
public:
	std::vector<GenerateItemData> insertVector;
	RE::TESForm* targetForm{}; // RE::TESLevItem*, RE::TESLevCharacter*, RE::TESLevSpell*
	uint8_t targetFormType{};
	GeneratedLeveledListInstruction instruction = GeneratedLeveledListInstruction::None;
};