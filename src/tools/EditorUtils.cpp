#include "EditorUtils.h"
#include <stdio.h>
#include <sstream>

std::string path = "";

// Gets the current directory path
// Beware of problems with mac or linux.
std::string getProjectDirectory() {
    
    TCHAR tpath[MAX_PATH + 1] = L"";
    DWORD len = GetCurrentDirectory(MAX_PATH, tpath);
    std::wstring t_path = tpath;
    std::string n_path = std::string(t_path.begin(), t_path.end());

    return n_path + "\\data\\";
}

// Gets all files and folders from path and reorders them, folders first.
NodeFile ProcessDirectoryOredered(std::string directory) {

    path = getProjectDirectory();
    NodeFile tmp_node = ProcessDirectory(directory);
    return ReorderNode(tmp_node);
}

// Method to reorder nodes, condition is folders first.
NodeFile ReorderNode(NodeFile tmp_node) {

    NodeFile f_node;
    f_node.name = tmp_node.name;

    for (int i = 0; i < tmp_node.children.size(); i++) {
        if (tmp_node.children[i].children.size() > 0) {
            f_node.children.push_back(tmp_node.children[i]);
            ReorderNode(tmp_node.children[i]);
        }
    }

    for (int i = 0; i < tmp_node.children.size(); i++) {
        if (tmp_node.children[i].children.size() == 0)
            f_node.children.push_back(tmp_node.children[i]);
    }

    return f_node;
}

// Standard c++ method to laod all data from directory structure.
NodeFile ProcessDirectory(std::string directory)
{
    std::string dirToOpen = path + directory;

    auto dir = opendir(dirToOpen.c_str());

    //set the new path for the content of the directory
    path = dirToOpen + "/";

    std::cout << "Process directory: " << dirToOpen.c_str() << std::endl;

    if (NULL == dir)
    {
        std::cout << "could not open directory: " << dirToOpen.c_str() << std::endl;
        return NodeFile();
    }

    auto entity = readdir(dir);

    NodeFile node;
    node.name = directory;

    while (entity != NULL)
    {
        NodeFile child = ProcessEntity(entity);
        node.children.push_back(child);
        entity = readdir(dir);
    }

    //we finished with the directory so remove it from the path
    path.resize(path.length() - 1 - directory.length());
    closedir(dir);

    return node;
}

NodeFile ProcessEntity(struct dirent* entity)
{
    //find entity type
    if (entity->d_type == DT_DIR)
    {//it's an direcotry
        //don't process the  '..' and the '.' directories
        if (entity->d_name[0] == '.')
        {
            return NodeFile();
        }

        //it's an directory so process it
        return ProcessDirectory(std::string(entity->d_name));
    }

    if (entity->d_type == DT_REG)
    {//regular file
        ProcessFile(std::string(entity->d_name));
        NodeFile node;
        node.name = entity->d_name;

        return node;
    }

    std::cout << "Not a file or directory: " << entity->d_name << std::endl;
}

void ProcessFile(std::string file)
{
    //std::cout << "Process file     : " << file.c_str() << std::endl;

    //if you want to do something with the file add your code here
}