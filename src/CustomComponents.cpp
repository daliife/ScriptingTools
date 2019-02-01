#pragma once
#include "CustomComponents.h"
#include "EntityComponentStore.h"
#include "extern.h"
#include <string.h>
//////////////////////////////////////////////////////////
////////////////// CUSTOM COMPONENTS /////////////////////
//////////////////////////////////////////////////////////

bool Tag::HasTag(const std::string & tag)
{
    for (auto& p : tags) {
        if (strcmp(p.c_str(), tag.c_str()) == 0)
            return true;
    }

    return false;
}

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

void Tag::getAllEntitiesByTag(const std::string & tag_str) {
        
    std::vector<int> entities;
    auto& all_tags = ECS.getAllComponents<Tag>();
    for (auto& tag : all_tags) {
        if (std::find(tag.tags.begin(), tag.tags.end(), tag_str) != tag.tags.end())
            entities.push_back(tag.owner);
    }
}

void Tag::Load(rapidjson::Value & entity, int ent_id) {

    auto& json_tags = entity["tags"].GetArray();
    std::vector<std::string> tags;
    for (auto& p : json_tags)
        tags.push_back(p.GetString());

    this->tags = tags;
}

void Tag::debugRender() {

    static char elevenBytes[11] = {};
    ImGui::AddSpace(0, 5);
    if (ImGui::TreeNode("Tags")) {
        ImGui::AddSpace(0, 5);
        for (auto& p : tags)
            ImGui::InputText("Tag", &p[0u], sizeof(elevenBytes));
        ImGui::TreePop();
    }

    ImDrawList*   draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
}

void Rotator::update(float dt) {

    if (ECS.entities.size() != 0) {

        Entity ent = ECS.entities[owner];
        Transform& rotator = ECS.getComponentFromEntity<Transform>(owner);
        rotator.rotateLocal(speed * dt, axis);
    }
}

void Rotator::Save(rapidjson::Document & json, rapidjson::Value & entity)
{
    rapidjson::Value obj(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType& allocator = json.GetAllocator();

    // Set translation
    {
        rapidjson::Value naxis(rapidjson::kArrayType);
        naxis.PushBack(axis.x, allocator);
        naxis.PushBack(axis.y, allocator);
        naxis.PushBack(axis.z, allocator);
        obj.AddMember("axis", naxis, allocator);
        obj.AddMember("speed", speed, allocator);
    }

    entity.AddMember("rotator", obj, allocator);
}

void Rotator::Load(rapidjson::Value & entity, int ent_id) {

    auto json_sp = entity["rotator"]["speed"].GetFloat();
    auto json_axis = entity["rotator"]["axis"].GetArray();

    speed = json_sp;
    axis = lm::vec3(json_axis[0].GetFloat(), json_axis[1].GetFloat(), json_axis[2].GetFloat());
}

void Rotator::debugRender() {

    ImGui::AddSpace(0, 5);
    if (ImGui::TreeNode("Rotator")) {
        ImGui::AddSpace(0, 5);
        ImGui::DragFloat3("Axis", &axis.x);
        ImGui::DragFloat("Speed", &speed);
        ImGui::TreePop();
    }

    ImDrawList*   draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
}