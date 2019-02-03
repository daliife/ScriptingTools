#pragma once
#include "comp_movingplatform.h"
#include "../EntityComponentStore.h"
#include "../extern.h"

void MovingPlatform::Save(rapidjson::Document & json, rapidjson::Value & entity)
{
	rapidjson::Value obj(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = json.GetAllocator();

	{
		rapidjson::Value ndir(rapidjson::kArrayType);
		ndir.PushBack(dir.x, allocator);
		ndir.PushBack(dir.y, allocator);
		ndir.PushBack(dir.z, allocator);
		obj.AddMember("dir", ndir, allocator);
		obj.AddMember("speed", speed, allocator);
	}

	entity.AddMember("movingplatform", obj, allocator);

}

void MovingPlatform::Load(rapidjson::Value & entity, int ent_id) {

	{
		auto json_speed = entity["movingplatform"]["speed"].GetFloat();
		auto json_dir = entity["movingplatform"]["dir"].GetArray();

		speed = json_speed;
		dir = lm::vec3(json_dir[0].GetFloat(), json_dir[1].GetFloat(), json_dir[2].GetFloat());

	}

}

void MovingPlatform::update(float dt) {

	if (ECS.entities.size() != 0) {
		Entity ent = ECS.entities[owner];
		Transform& platform = ECS.getComponentFromEntity<Transform>(owner);
		platform.translate(dir.normalize() * speed * dt);
	}

}

void MovingPlatform::debugRender() {

    ImGui::AddSpace(0, 5);
    if (ImGui::TreeNode("MovingPlatform")) {
        ImGui::AddSpace(0, 5);


        ImGui::TreePop();
    }

    // Adds a cool line at the end.
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
}