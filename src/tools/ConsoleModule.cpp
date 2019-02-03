#include "ConsoleModule.h"
#include "../imgui_demo.cpp"
#include "../extern.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "../Game.h"

#define dmin(a,b)            (((a) < (b)) ? (a) : (b))
#define dmax(a,b)            (((a) > (b)) ? (a) : (b))

// Portable helpers
int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
char* Strdup(const char *str) { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }
void  Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

// Method to split commands
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(txt.substr(initialPos, dmin(pos, txt.size()) - initialPos + 1));

    return strs.size();
}

ConsoleModule::ConsoleModule()
{
    init();
}

ConsoleModule::~ConsoleModule()
{

}

// Initialize the console with some basic commands.
void ConsoleModule::init()
{
    text_color_ = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    copy_clipboard_ = false;
    historypos_ = -1;

    ConsoleClear();
    memset(inputbuf_, 0, sizeof(inputbuf_));
    commands_.push_back("/help");
    commands_.push_back("/history");
    commands_.push_back("/clear");
    commands_.push_back("/classify");
	commands_.push_back("moveplayer");
	commands_.push_back("colorbackground");
	commands_.push_back("debug");
	commands_.push_back("changecamera");
    ConsoleWrite(true, "Console Initialized!");
}

// Clear all console registered commands
void ConsoleModule::ConsoleClear()
{
    for (int i = 0; i < items_.size(); i++)
        free(items_[i]);

    items_.clear();
    scrollbottom_ = true;
}

// Write into console without time stamps
void ConsoleModule::ConsoleWrite(const char* fmt, ...)
{
    // Write final message to char array
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);

    items_.push_back(Strdup(buf));
    scrollbottom_ = true;
}

// Write into console with time stamps
void ConsoleModule::ConsoleWrite(bool timestamp, const char* fmt, ...)
{
    // Retrieve current time
    std::string tmp;
    if (timestamp) {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "[%d/%m/%Y %H:%M:%S] Debug: ") << fmt;
        tmp = oss.str();
        fmt = tmp.c_str();
    }
    
    // Write final message to char array
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);

    items_.push_back(Strdup(buf));
    scrollbottom_ = true;
}

// Update the console
void ConsoleModule::update(float dt)
{
    // TODO: display items starting from the bottom

    // Top menu buttons used to clear, copy and scroll window
    {
        if (ImGui::SmallButton("Clear")) { ConsoleClear(); } ImGui::SameLine();
        copy_clipboard_ = ImGui::SmallButton("Copy"); ImGui::SameLine();
        if (ImGui::SmallButton("Scroll to bottom")) scrollbottom_ = true;
		ImGui::SameLine();
		if (Game::get().game_instance->getDebugSystem().isActive()) {
			if (ImGui::SmallButton("Deactivate debug")) {
				Game::get().game_instance->getDebugSystem().setActive(false);
				ConsoleWrite(false, "Debug Off");
			}
		}
		else {
			if (ImGui::SmallButton("Activate debug")) {
				Game::get().game_instance->getDebugSystem().setActive(true);
				ConsoleWrite(false, "Debug On");
			}
		}
    }

    // Filters
    ImGui::Separator();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    static ImGuiTextFilter filter;
    filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::PopStyleVar();
    ImGui::Separator();
    {
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ConsoleClear();
            ImGui::EndPopup();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_clipboard_)
            ImGui::LogToClipboard();

        // Loop through all the items, check if they contain the filter provided by the user
        ImVec4 text_color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        for (int i = 0; i < items_.size(); i++)
        {
            const char* item = items_[i];
            if (!filter.PassFilter(item))
                continue;

            if (strstr(item, "[error]")) text_color = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
            else text_color = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, text_color);
            ImGui::TextUnformatted(item);
            ImGui::PopStyleColor();
        }

        if (copy_clipboard_)
            ImGui::LogFinish();

        if (scrollbottom_)
            ImGui::SetScrollHereY(1.0f);

        scrollbottom_ = false;
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line input
    // Used to introduce the commands into the engine
    {
        bool reclaim_focus = false;
        if (ImGui::InputText("Input", inputbuf_, IM_ARRAYSIZE(inputbuf_), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
        {
            char* s = inputbuf_;
            Strtrim(s);
            if (s[0])
                ExecCommand(s);

            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }
}

int ConsoleModule::TextEditCallbackStub(ImGuiInputTextCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
{
    ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
    return console->TextEditCallback(data);
}

void ConsoleModule::ExecCommand(const char* command_line)
{
    ConsoleWrite(false,"%s\n", command_line);

    historypos_ = -1;
    for (int i = history_.Size - 1; i >= 0; i--) {
        if (Stricmp(history_[i], command_line) == 0)
        {
            free(history_[i]);
            history_.erase(history_.begin() + i);
            break;
        }
    }

    history_.push_back(Strdup(command_line));

    // Process static commands command
    if (Stricmp(command_line, "/clear") == 0)
    {
        ConsoleClear();
    }
    else if (Stricmp(command_line, "/help") == 0)
    {
        ConsoleWrite(false, "Commands:");
        for (int i = 0; i < commands_.Size; i++)
            ConsoleWrite(false, "- %s", commands_[i]);
    }
    else if (Stricmp(command_line, "/history") == 0)
    {
        int first = history_.Size - 10;
        for (int i = first > 0 ? first : 0; i < history_.Size; i++)
            ConsoleWrite(false, "%3d: %s\n", i, history_[i]);
    }
    else 
	{
        BuildCommand(command_line);
    }
}

// Use this method to implement your custom commands
// Link these commands to the script system.
// https://nwnlexicon.com/index.php?title=Category:Beginning_Scripting
// https://scripts.zeroy.com/
void ConsoleModule::BuildCommand(const char* cmd)
{
    // Getting the command and splitting the string by blankspace
    std::string input = cmd;
    std::vector<std::string> v; // Vector used to hold the different parameters
    split(cmd, v, ' ');

	bool com_found = false;

    if (input.find("moveplayer") != std::string::npos)
    {
        float posx = atof(v[1].c_str());
        float posy = atof(v[2].c_str());
        float posz = atof(v[3].c_str());
        int ent_id = ECS.getEntity("PlayerFPS");
        Camera& camera = ECS.getComponentInArray<Camera>(ECS.main_camera);
        Transform& transform = ECS.getComponentFromEntity<Transform>(ent_id);
        transform.translate( lm::vec3(posx, posy, posz));
        camera.position = lm::vec3(posx, posy, posz);
		com_found = true;
    }
	
	if (input.find("colorbackground") != std::string::npos)
	{
		float r = atof(v[1].c_str());
		float g = atof(v[2].c_str());
		float b = atof(v[3].c_str());
		Game::get().game_instance->getGraphicsSystem().setClearColor(lm::vec3(r, g, b));
		com_found = true;
	}

	if (input.find("debug") != std::string::npos)
	{
		float state = atof(v[1].c_str());
		if (state == 1) {
			Game::get().game_instance->getDebugSystem().setActive(true);
		} else if(state == 0) {
			Game::get().game_instance->getDebugSystem().setActive(false);
		}else {
			ConsoleWrite(false, "Invalid Parameter: can only be 0(off) or 1(on).");
		}
		com_found = true;
	}
	
	if (input.find("changecamera") != std::string::npos)
	{
		//TODO: Change control type through call
		//Game::get().game_instance->getControlSystem().control_type;
		float num_cam = atof(v[1].c_str());
		float total_cam = ECS.getAllComponents<Camera>().size();
		if (total_cam > num_cam) {
			ECS.main_camera = num_cam;
		} else {
			ConsoleWrite(false, "Invalid Parameter: Not enough cameras on scene.");
		}
		com_found = true;
	}

	if (!com_found) {
        ConsoleWrite(false, "Unknown command: '%s'\n", cmd);
    }

}

// Callbacks from the console input to save information.
int ConsoleModule::TextEditCallback(ImGuiInputTextCallbackData* data)
{
    switch (data->EventFlag)
    {
        case ImGuiInputTextFlags_CallbackCompletion:
        {
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (int i = 0; i < commands_.Size; i++)
                if (Strnicmp(commands_[i], word_start, (int)(word_end - word_start)) == 0)
                    candidates.push_back(commands_[i]);

            if (candidates.Size == 0)
            {
                // No match
                ConsoleWrite(false, "No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
            }
            else if (candidates.Size == 1)
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            }
            else
            {
                // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
                int match_len = (int)(word_end - word_start);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                ConsoleWrite(false, "Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                    ConsoleWrite(false, "- %s\n", candidates[i]);
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            // Example of HISTORY
            const int prev_history_pos = historypos_;
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (historypos_ == -1)
                    historypos_ = history_.Size - 1;
                else if (historypos_ > 0)
                    historypos_--;
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (historypos_ != -1)
                    if (++historypos_ >= history_.Size)
                        historypos_ = -1;
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != historypos_)
            {
                const char* history_str = (historypos_ >= 0) ? history_[historypos_] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
    }
    return 0;
}