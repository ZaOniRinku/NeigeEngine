#pragma once
#pragma warning(disable:4996)
#include "../NeigeDefines.h"
#include "FileTools.h"

struct ModelLoader {
	static void load(const std::string& filePath);
	static void loadglTF(const std::string& filePath);
};