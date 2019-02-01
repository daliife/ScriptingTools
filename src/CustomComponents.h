#pragma once
#include "includes.h"
#include "Components.h"

//////////////////////////////////////////////////////////
////////////////// CUSTOM COMPONENTS /////////////////////
//////////////////////////////////////////////////////////

// Implement this class using scripting system.
struct Tag : public Component {

    std::vector<std::string> tags;

    bool HasTag(const std::string & tag);
    void Save(rapidjson::Document& json, rapidjson::Value & entity);
    void getAllEntitiesByTag(const std::string & tag_str);
    void Load(rapidjson::Value & entity, int ent_id);
    void debugRender();
};

// Implement this class using scripting system.
struct Rotator : public Component {

    float speed = 1;
    lm::vec3 axis = lm::vec3(0,1,0);

    void update(float dt);
    void Save(rapidjson::Document& json, rapidjson::Value & entity);
    void Load(rapidjson::Value & entity, int ent_id);
    void debugRender();
};
