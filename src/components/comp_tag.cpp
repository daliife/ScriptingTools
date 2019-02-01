#pragma once
#include "comp_tag.h"
#include "../EntityComponentStore.h"
#include "../extern.h"

//////////////////////////////////////////////////////////
////////////////// CUSTOM COMPONENTS /////////////////////
//////////////////////////////////////////////////////////

// Method used to know if the tag given belongs to an object
bool Tag::HasTag(const std::string & tag)
{
    for (auto& p : tags) {
        if (strcmp(p.c_str(), tag.c_str()) == 0)
            return true;
    }

    return false;
}

// Method to save this component into json file
void Tag::Save(rapidjson::Document & json, rapidjson::Value & entity)
{
    rapidjson::Document::AllocatorType& allocator = json.GetAllocator();

    // Set translation
    {
        rapidjson::Value ntags(rapidjson::kArrayType);
        for (auto tag : tags) {
            rapidjson::Value val(rapidjson::kObjectType);
            val.SetString(tag.c_str(), static_cast<rapidjson::SizeType>(tag.length()), allocator);
            ntags.PushBack(val, allocator);
        }

        entity.AddMember("tags", ntags, allocator);
    }
}

// Returns all entities of a given tag type.
void Tag::getAllEntitiesByTag(const std::string & tag_str) {

    std::vector<int> entities;
    auto& all_tags = ECS.getAllComponents<Tag>();
    for (auto& tag : all_tags) {
        if (std::find(tag.tags.begin(), tag.tags.end(), tag_str) != tag.tags.end())
            entities.push_back(tag.owner);
    }
}

// Load the tag
void Tag::Load(rapidjson::Value & entity, int ent_id) {

    auto& json_tags = entity["tags"].GetArray();
    std::vector<std::string> tags;
    for (auto& p : json_tags)
        tags.push_back(p.GetString());

    this->tags = tags;
}

// Render debug the tag.
void Tag::debugRender() {

    static char elevenBytes[11] = {};
    ImGui::AddSpace(0, 5);
    if (ImGui::TreeNode("Tags")) {
        ImGui::AddSpace(0, 5);
        for (auto& p : tags)
            ImGui::InputText("Tag", &p[0u], sizeof(elevenBytes));
        ImGui::TreePop();
    }

    // Adds a cool line at the end.
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
}