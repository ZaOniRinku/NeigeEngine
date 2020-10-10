#include "FileTools.h"

std::vector<char> FileTools::read(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::ate);
    if (!file.is_open()) {
        NEIGE_ERROR("File \"" + filePath + "\" could not be opened.");
    }
    size_t size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);
    file.close();
    return buffer;
}

std::string FileTools::extension(const std::string& filePath) {
    std::size_t dotPosition = filePath.find('.');
    if (dotPosition == std::string::npos) {
        NEIGE_ERROR("File \"" + filePath + "\" extension could not be found.");
    }
    return filePath.substr(dotPosition + 1);
}