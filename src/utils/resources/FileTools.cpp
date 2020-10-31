#include "FileTools.h"

std::string FileTools::readAscii(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::ate);
    if (!file.is_open()) {
        NEIGE_ERROR("File \"" + filePath + "\" could not be opened (ASCII).");
    }
    file.seekg(0);
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return fileContent;
}

std::string FileTools::readBinary(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        NEIGE_ERROR("File \"" + filePath + "\" could not be opened (Binary).");
    }
    file.seekg(0);
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return fileContent;
}

std::string FileTools::filename(const std::string& filePath) {
    std::size_t slashPosition = filePath.find_last_of('/');
    if (slashPosition == std::string::npos) {
        return filePath;
    }
    return filePath.substr(slashPosition + 1);
}

std::string FileTools::extension(const std::string& filePath) {
    std::size_t dotPosition = filePath.find_last_of('.');
    if (dotPosition == std::string::npos) {
        NEIGE_ERROR("File \"" + filePath + "\" extension could not be found.");
    }
    return filePath.substr(dotPosition + 1);
}

std::string FileTools::fileGetDirectory(const std::string& filePath) {
    std::size_t slashPosition = filePath.find_last_of('/');
    if (slashPosition == std::string::npos) {
        return filePath;
    }
    return filePath.substr(0, slashPosition + 1);
}
