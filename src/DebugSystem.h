#pragma once
#include "includes.h"
#include "Shader.h"
#include <vector>

class DebugSystem {
public:
	~DebugSystem();
	void lateInit();
	void update(float dt);

	void setActive(bool a);

	//public imGUI functions
    bool isRenderActive() { return render_active; };
	bool isShowGUI() { return show_imGUI_; };
	void toggleimGUI() { show_imGUI_ = !show_imGUI_; };

	//set picking ray
	void setPickingRay(int mouse_x, int mouse_y, int screen_width, int screen_height);

private:
	//bools to draw or not
	bool draw_grid_;
	bool draw_icons_;
	bool draw_frustra_;
	bool draw_colliders_;

	//cube for frustra and boxes
	void createCube_();
	GLuint cube_vao_;

	//colliders
	void createRay_();
	GLuint collider_ray_vao_;
	GLuint collider_box_vao_;

	//icons
	void createIcon_();
	GLuint icon_vao_;
	GLuint icon_light_texture_;
	GLuint icon_camera_texture_;

	//grid
	void createGrid_();
	GLuint grid_vao_;
	GLuint grid_num_indices;
	float grid_colors[12] = {
		0.7f, 0.7f, 0.7f, //grey
		1.0f, 0.5f, 0.5f, //red
		0.5f, 1.0f, 0.5f, //green
		0.5f, 0.5f, 1.0f }; //blue

							//shaders
	Shader* grid_shader_;
	Shader* icon_shader_;

	//imGUI
	bool show_imGUI_ = true;
    bool render_active = false;
	void updateimGUI_(float dt);

	//picking
	bool can_fire_picking_ray_ = true;
	int ent_picking_ray_;

};

