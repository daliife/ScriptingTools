#pragma once
#include "Components.h"
#include "Game.h"
#include "extern.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

/**** COMPONENTS ****/

// Used to save the transform object into json
void Transform::Save(rapidjson::Document& json, rapidjson::Value & entity)
{
    rapidjson::Value obj(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType& allocator = json.GetAllocator();

    // Set translation
    {
        rapidjson::Value translation(rapidjson::kArrayType);
        translation.PushBack(position().x, allocator);
        translation.PushBack(position().y, allocator);
        translation.PushBack(position().z, allocator);
        obj.AddMember("translation", translation, allocator);
    }

    // Set rotation
    {
        rapidjson::Value rotation(rapidjson::kArrayType);
        rotation.PushBack(m[8], allocator);
        rotation.PushBack(m[9], allocator);
        rotation.PushBack(m[10], allocator);
        obj.AddMember("rotation", rotation, allocator);
    }

    // Set scale
    {
        rapidjson::Value scale(rapidjson::kArrayType);
        scale.PushBack(m[0], allocator);
        scale.PushBack(m[5], allocator);
        scale.PushBack(m[10], allocator);
        obj.AddMember("scale", scale, allocator);
    }

    // Finally adding the transform component to the json.
    entity.AddMember("transform", obj, allocator);
}

void Transform::Load(rapidjson::Value & entity, int ent_id) {

    auto jt = entity["transform"]["translation"].GetArray();
    auto jr = entity["transform"]["rotation"].GetArray();
    auto js = entity["transform"]["scale"].GetArray();

    lm::vec3 rotate; rotate.x = jr[0].GetFloat(); rotate.y = jr[1].GetFloat(); rotate.z = jr[2].GetFloat();
    lm::quat qR(rotate.x*DEG2RAD, rotate.y*DEG2RAD, rotate.z*DEG2RAD);
    lm::mat4 R;
    R.rotateLocal(rotate.x*DEG2RAD, lm::vec3(1, 0, 0));
    R.rotateLocal(rotate.y*DEG2RAD, lm::vec3(0, 1, 0));
    R.rotateLocal(rotate.z*DEG2RAD, lm::vec3(0, 0, 1));
    this->set(*this * R);
    this->scaleLocal(js[0].GetFloat(), js[1].GetFloat(), js[2].GetFloat());
    this->translate(jt[0].GetFloat(), jt[1].GetFloat(), jt[2].GetFloat());
}

// Method to debug render the parameters of the transform component
// https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati
void Transform::debugRender() {

    lm::vec3 pos = this->position();
    lm::vec3 front = this->front();
    float pos_array[3] = { pos.x, pos.y, pos.z };
    float rot_array[3] = { m[8], m[9], m[10] };
    float scal_array[3] = { m[0], m[5], m[10] };

    ImGui::AddSpace(0, 5);
    {
        if (ImGui::TreeNode("Transform")) {
            ImGui::AddSpace(0, 5);
            if (ImGui::DragFloat3("Position", pos_array)) {
                this->position(pos_array[0], pos_array[1], pos_array[2]);
            }

            if (ImGui::DragFloat3("Rotation", rot_array)) {
                m[8] = rot_array[0];
                m[9] = rot_array[1];
                m[10] = rot_array[2];
            }

            if (ImGui::DragFloat3("Scale", scal_array)) {
                m[0] = scal_array[0];
                m[5] = scal_array[1];
                m[10] = scal_array[2];
            }
            ImGui::TreePop();
        }
    }
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
}


void Mesh::Save(rapidjson::Document& json, rapidjson::Value & entity)
{
    rapidjson::Value obj(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType& allocator = json.GetAllocator();

    std::string mesh_name = Game::get().getGraphicsSystem().geometries_[geometry].name;
    std::string material_name = Game::get().getGraphicsSystem().getMaterial(material).name;
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.SetString(mesh_name.c_str(), static_cast<rapidjson::SizeType>(mesh_name.length()), allocator);
        obj.AddMember("mesh", val, allocator);

        rapidjson::Value materials(rapidjson::kArrayType);
        rapidjson::Value val2(rapidjson::kObjectType);
        val2.SetString(material_name.c_str(), static_cast<rapidjson::SizeType>(material_name.length()), allocator);
        materials.PushBack(val2, allocator);
        obj.AddMember("materials", materials, allocator);
    }

    entity.AddMember("render", obj, allocator);
}

void Mesh::Load(rapidjson::Value & entity, int ent_id) {
 
}
// Debug render
// Implement more parameters
// TO-DO
void Mesh::debugRender() {

    ImGui::AddSpace(0, 5);

    if (ImGui::TreeNode("Renderer")) {
        ImGui::AddSpace(0, 5);
        ImGui::Text("Mesh:");
        ImGui::SameLine();
        std::string mesh_name = Game::get().getGraphicsSystem().geometries_[geometry].name;
        ImGui::Text(mesh_name.c_str());

        ImGui::Unindent(8);
        if (ImGui::TreeNode("Material" )) {
        
            Material & mat = Game::get().getGraphicsSystem().getMaterial(material);
            std::string material_name = mat.name;

			ImGui::AddSpace(0, 10);
			ImGui::Text("File:");
			ImGui::SameLine();
			ImGui::Text(material_name.c_str());
			ImGui::Image((ImTextureID)(mat.diffuse_map), ImVec2(64, 64));
			ImGui::SameLine();
			ImGui::SetCursorPos({ ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (64 - ImGui::GetFont()->FontSize) / 2 });
			ImGui::Text(" Albedo Material");

			float ambient[3] = { mat.ambient.x, mat.ambient.y, mat.ambient.z };
			if (ImGui::DragFloat3("Ambient", ambient)) {
				mat.ambient.x = ambient[0];
				mat.ambient.y = ambient[1];
				mat.ambient.z = ambient[2];
			}

			float specular[3] = { mat.specular.x, mat.specular.y, mat.specular.z };
			if (ImGui::DragFloat3("Specular", specular)) {
				mat.specular.x = specular[0];
				mat.specular.y = specular[1];
				mat.specular.z = specular[2];
			}

			float diffuse[3] = { mat.diffuse.x, mat.diffuse.y, mat.diffuse.z };
			if (ImGui::DragFloat3("Diffuse", diffuse)) {
				mat.diffuse.x = diffuse[0];
				mat.diffuse.y = diffuse[1];
				mat.diffuse.z = diffuse[2];
			}

			ImGui::DragFloat("Specular Gloss", &mat.specular_gloss);


			// TO-DO Debug material
            // Add backface culling properties
			ImGui::AddSpace(0, 10);
			ImGui::Text("Backface Culling");
			ImGui::Text("fjsdhkfhksjfhjks");
			ImGui::Text("fjsdhkfhksjfhjks");
			ImGui::Text("fjsdhkfhksjfhjks");

			// Add more textures
			// Add tiling
			ImGui::AddSpace(0, 10);
			ImGui::Text("Tiling");
			lm::vec3 pos(0.0f,0.0f,0.0f);
			float pos_array[3] = { pos.x, pos.y, pos.z };
			if (ImGui::DragFloat3("Value", pos_array)) {
				pos = lm::vec3(pos_array[0], pos_array[1], pos_array[2]);
			}

			ImGui::AddSpace(0, 10);

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImDrawList*   draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
}

//Light Component
// - id of transform in ECS array
// - color of light
//Later will be developed extensively


void Light::Save(rapidjson::Document& json, rapidjson::Value & entity)
{
    // TO-DO
    // Implement save on lights
}

void Light::Load(rapidjson::Value & entity, int ent_id) {

    auto json_lc = entity["light"]["color"].GetArray();
    this->color = lm::vec3(json_lc[0].GetFloat(), json_lc[1].GetFloat(), json_lc[2].GetFloat());
}

void Light::debugRender() {

    ImGui::AddSpace(0, 5);
    {
        if (ImGui::TreeNode("Light")) {
            ImGui::AddSpace(0, 5);
            ImGui::ColorPicker4("Color", &color.x);
            ImGui::TreePop();
        }
    }
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
}

//ColliderComponent. Only specifies size - collider location is given by any
//associated TransformComponent
// - collider_type is the type according to enum above
// - local_center is offset from transform
// - local_halfwidth is used for box,
// - direction is used for ray
// - max_distance is used to convert ray to segment


void Collider::Save(rapidjson::Document& json, rapidjson::Value & entity)
{
    rapidjson::Value obj(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType& allocator = json.GetAllocator();

    {
        obj.AddMember("type", "box", allocator);
        obj.AddMember("group", "All", allocator);
        obj.AddMember("mask", "", allocator);

        obj.AddMember("trigger", false, allocator);
        obj.AddMember("dynamic", false, allocator);
        obj.AddMember("controller", false, allocator);
        obj.AddMember("gravity", false, allocator);
    }

    entity.AddMember("collider", obj, allocator);
}

void Collider::Load(rapidjson::Value & entity, int ent_id) {

    std::string coll_type = entity["collider"]["type"].GetString();
    if (coll_type == "box") {
        this->collider_type = ColliderTypeBox;

        auto json_col_center = entity["collider"]["center"].GetArray();
        this->local_center.x = json_col_center[0].GetFloat();
        this->local_center.y = json_col_center[1].GetFloat();
        this->local_center.z = json_col_center[2].GetFloat();

        auto json_col_halfwidth = entity["collider"]["halfwidth"].GetArray();
        this->local_halfwidth.x = json_col_halfwidth[0].GetFloat();
        this->local_halfwidth.y = json_col_halfwidth[1].GetFloat();
        this->local_halfwidth.z = json_col_halfwidth[2].GetFloat();
    }
}

void Collider::debugRender() {

    const char* items[] = { "Box" };
    static const char* current_item = 0;
    current_item = items[0];

    ImGui::AddSpace(0, 5);
    {
        if (ImGui::TreeNode("Collider")) {
            ImGui::AddSpace(0, 5);
            if (ImGui::BeginCombo("Type", current_item))
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    bool is_selected = (current_item == items[n]);
                    if (ImGui::Selectable(items[n], is_selected))
                        current_item = items[n];
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::DragFloat3("Center", &local_center.x);
            ImGui::DragFloat3("Halfwidth", &local_halfwidth.x);
            ImGui::TreePop();
        }
    }
    ImDrawList*   draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
}

// Used to save the current entity and its components into a json object

void Entity::Save(rapidjson::Document& json, rapidjson::Value & entity) {

    // Allocator for the json
    rapidjson::Document::AllocatorType& allocator = json.GetAllocator();

    // Set name to the entity
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.SetString(name.c_str(), static_cast<rapidjson::SizeType>(name.length()), allocator);
        entity.AddMember("name", val, allocator);
    }

    ////entity["entities"][index]["name"] = name;
    auto& transform = ECS.getSafeComponentFromEntity<Transform>(name);
    auto& mesh = ECS.getSafeComponentFromEntity<Mesh>(name);
    auto& light = ECS.getSafeComponentFromEntity<Light>(name);
    auto& collider = ECS.getSafeComponentFromEntity<Collider>(name);
    auto& rotator = ECS.getSafeComponentFromEntity<Rotator>(name);
    auto& tag = ECS.getSafeComponentFromEntity<Tag>(name);
	auto& movable = ECS.getSafeComponentFromEntity<MovingPlatform>(name);

    if (transform.index != 0) transform.Save(json, entity);
    if (mesh.index != 0) mesh.Save(json, entity);
    if (light.index != 0) light.Save(json, entity);
    if (collider.index != 0) collider.Save(json, entity);
    if (rotator.index != 0) rotator.Save(json, entity);
    if (tag.index != 0) tag.Save(json, entity);
}