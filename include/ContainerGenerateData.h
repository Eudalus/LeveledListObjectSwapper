#pragma once

#include "ContainerGenerateItemData.h"

class ContainerGenerateData
{
public:
	RE::TESLevItem* leveledList;
	std::vector<ContainerGenerateItemData> vector;
	size_t processCounter;
};