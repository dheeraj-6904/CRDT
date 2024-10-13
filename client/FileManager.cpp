#include "FileManager.h"
#include <fstream>
#include <iostream>

FileManager::FileManager() {}

FileManager::~FileManager() {}

bool FileManager::saveFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
        return true;
    }
    return false;
}

bool FileManager::deleteFile(const std::string& filename) {
    if (remove(filename.c_str()) == 0) {
        return true;
    }
    return false;
}

std::string FileManager::openFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return content;
    }
    return "";
}
