#pragma once
#include "../includes.h"
#include "../Components.h"

//////////////////////////////////////////////////////////
////////////////// CUSTOM COMPONENTS /////////////////////
//////////////////////////////////////////////////////////

// Method used to rotate object around axis given an speed
struct Rotator : public Component {

    float speed = 1;
    lm::vec3 axis = lm::vec3(0, 1, 0);

    void update(float dt);
    void Save(rapidjson::Document& json, rapidjson::Value & entity);
    void Load(rapidjson::Value & entity, int ent_id);
    void debugRender();
};
