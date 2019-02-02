#pragma once
#include "../includes.h"
#include "../Shader.h"
#include <vector>

// Structure to hold parent children relationship
struct TransformNode {
    std::vector<TransformNode> children;
    int trans_id;
    int entity_owner;
    std::string ent_name;
    bool isTop = false;
};

class NodeFile;
class ConsoleModule;
class EditorGraphModule;

// This class holds the user interface and other related methods
class EditorSystem {
public:

    lm::vec2 render_size;
    static EditorSystem* editor_instance;

    EditorSystem();
    ~EditorSystem();

    void Init();
    void SetStyles();
    void update(float dt);
    void SetPickingRay(int mouse_x, int mouse_y, int screen_width, int screen_height);

    static EditorSystem& get() {
        assert(editor_instance);
        return *editor_instance;
    }

    bool GetEditorStatus() {
        return is_editor_mode;
    }

    void SetEditorStatus(bool mode) {
        is_editor_mode = mode;
    }

    bool GetRenderStatus() {
        return is_render_active;
    }

    void ToggleEditorMode() { 
        is_editor_mode = !is_editor_mode;
    }

private:
	//bools to draw or not
    bool is_editor_mode;
    bool is_render_active;
    bool is_adding_component;
    bool is_saving_scene;

    void RenderNode(TransformNode & trans);
    void RenderProject(NodeFile & trans);

    void UpdateMenubar(float dt);
    void UpdateRender(float dt);
    void UpdateInspector(float dt);
    void UpdateHierarchy(float dt);
    void UpdateProject(float dt);
    void UpdateConsole(float dt);
    void UpdateFPS(float dt);
    
    void UpdateComponentMenu(float dt);
    void AddComponentSelected(int id, int entity_id);
	void DeleteEntityScene(int entity_id);
    void SaveSceneToFile(const std::string & scene_name);

    std::string selected;
    int ent_picking_ray_;

    ConsoleModule * console_module_;
    EditorGraphModule * graph_module_;
};

