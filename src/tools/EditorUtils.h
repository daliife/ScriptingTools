#ifndef EDITOR_UTILS_H
#define EDITOR_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include "dirent.h"

// Util methods
struct NodeFile;

// Structure to iteate directory trees
struct NodeFile {

    std::string name;
    std::vector<NodeFile> children;
};

std::string getProjectDirectory();
void ProcessFile(std::string file);
NodeFile ReorderNode(NodeFile tmp_node);
NodeFile ProcessDirectoryOredered(std::string directory);
NodeFile ProcessDirectory(std::string directory);
NodeFile ProcessEntity(struct dirent* entity);

#endif