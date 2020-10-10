#pragma once
#include "NeigeDefines.h"
#include <fstream>
#include <vector>
#include <string>

struct FileTools {
	static std::vector<char> read(const std::string& filePath);
	static std::string extension(const std::string& filePath);
};
