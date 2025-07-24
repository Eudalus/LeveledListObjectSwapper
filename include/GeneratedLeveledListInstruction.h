#pragma once

enum GeneratedLeveledListInstruction : uint8_t
{
	None = 0, // generated leveled list inserted as protocol 153
	CountOne = 1 << 0, // generated leveled list insert as protocol 152 or 150 with LevelOne set
	LevelOne = 1 << 1, // generated leveled list insert as protocol 151 or 150 with CountOne set
	AddUseAllFlag = 1 << 2, // add UseAll flag to generated leveled list

	// NOT ALLOWED FOR KEYWORD TARGETS
	ReinsertTargetOnce = 1 << 3, // add target when generating leveled list, guaranteed to insert target item once into the generated leveled list if size allows
	ReinsertTargetEachTime = 1 << 4, // for each insert item in generated leveled list, reinsert target item if size allows
};