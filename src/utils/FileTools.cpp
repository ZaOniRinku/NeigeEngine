#include "FileTools.h"

std::vector<char> FileTools::readAscii(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::ate);
    if (!file.is_open()) {
        NEIGE_ERROR("File \"" + filePath + "\" could not be opened (ASCII).");
    }
    size_t size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);
    file.close();
    return buffer;
}

std::vector<char> FileTools::readBinary(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        NEIGE_ERROR("File \"" + filePath + "\" could not be opened (Binary).");
    }
    size_t size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);
    file.close();
    return buffer;
}

std::string FileTools::extension(const std::string& filePath) {
    std::size_t dotPosition = filePath.find_last_of('.');
    if (dotPosition == std::string::npos) {
        NEIGE_ERROR("File \"" + filePath + "\" extension could not be found.");
    }
    return filePath.substr(dotPosition + 1);
}