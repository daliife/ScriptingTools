#pragma once
#include "comp_rotator.h"
#include "../EntityComponentStore.h"
#include "../extern.h"

//////////////////////////////////////////////////////////
////////////////// CUSTOM COMPONENTS /////////////////////
//////////////////////////////////////////////////////////


// Important function, this causes the rotation.
void Rotator::update(float dt) {

    if (ECS.entities.size() != 0) {

        Entity ent = ECS.entities[owner];
        Transform& rotator = ECS.getComponentFromEntity<Transform>(owner);
        rotator.rotateLocal(speed * dt, axis);
    }
}

// Save to json
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

// Load method from json
void Rotator::Load(rapidjson::Value & entity, int ent_id) {

    auto json_sp = entity["rotator"]["speed"].GetFloat();
    auto json_axis = entity["rotator"]["axis"].GetArray();

    speed = json_sp;
    axis = lm::vec3(json_axis[0].GetFloat(), json_axis[1].GetFloat(), json_axis[2].GetFloat());
}

// Debug on ImGui
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