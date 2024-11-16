#pragma once

#include "ContainerGenerateItemData.h"

class ContainerGenerateData
{
public:
	RE::TESLevItem* leveledList = nullptr;
	std::vector<ContainerGenerateItemData> vector;
	//size_t processCounter;
};