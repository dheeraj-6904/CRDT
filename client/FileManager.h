#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>

class FileManager {
public:
    FileManager();
    ~FileManager();
    
    bool saveFile(const std::string& filename, const std::string& content);
    bool deleteFile(const std::string& filename);
    std::string openFile(const std::string& filename);
    
    std::vector<std::string> getFilesFromServer();
};

#endif
