#pragma once
#include "../includes.h"
#include "../Shader.h"
#include <vector>

#define COMMAND_SIZE 128

// Console module
// Used to execute commands and extra functionality
class ConsoleModule {
public:

    ConsoleModule();
    ~ConsoleModule();

    void init();
    void update(float dt);

    void ConsoleClear();
    void ConsoleWrite(const char * fmt, ...);
    void ConsoleWrite(bool timestamp, const char* fmt, ...);

private:

    struct script_command {
        std::string code;
        std::vector<std::string> arguments;
    };

    int historypos_;
    char inputbuf_[256];
    bool scrollbottom_;
    bool copy_clipboard_;

    ImVec4 text_color_;
    ImVector<char*> items_;
    ImVector<char*> history_;
    ImVector<const char*> commands_;

    void ExecCommand(const char* command_line);
    void BuildCommand(const char* cmd);

    int TextEditCallback(ImGuiInputTextCallbackData* data);
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
};

