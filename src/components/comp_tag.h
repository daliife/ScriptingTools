#pragma once
#include "../includes.h"
#include "../Components.h"

//////////////////////////////////////////////////////////
////////////////// CUSTOM COMPONENTS /////////////////////
//////////////////////////////////////////////////////////

// Tag class, used to determine type of objects

struct Tag : public Component {

    std::vector<std::string> tags;

    bool HasTag(const std::string & tag);
    void Save(rapidjson::Document& json, rapidjson::Value & entity);
    void Load(rapidjson::Value & entity, int ent_id);

    void getAllEntitiesByTag(const std::string & tag_str);
    void debugRender();
};