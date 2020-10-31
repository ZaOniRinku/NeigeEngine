#pragma once
#include "../NeigeDefines.h"
#include <fstream>
#include <vector>
#include <string>

struct FileTools {
	static std::string readAscii(const std::string& filePath);
	static std::string readBinary(const std::string& filePath);
	static std::string filename(const std::string& filePath);
	static std::string extension(const std::string& filePath);
	static std::string fileGetDirectory(const std::string& filePath);
};