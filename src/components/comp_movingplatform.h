#pragma once
#include "../includes.h"
#include "../Components.h"

struct MovingPlatform : public Component {

	float speed = 1.0f;
	lm::vec3 dir = lm::vec3(0.0f, 1.0f, 0.0f);

    void Save(rapidjson::Document& json, rapidjson::Value & entity);
    void Load(rapidjson::Value & entity, int ent_id);
	void update(float dt);
	
	void debugRender();

};