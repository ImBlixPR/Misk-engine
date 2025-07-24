// scene_manager.cpp
#include "mkpch.h"
#include "scene_manager.h"
#include "renderer/render_data.h"

namespace scene {
    std::unordered_map<std::string, std::shared_ptr<Scene>> Scene_manager::scenes;
    std::shared_ptr<Scene> Scene_manager::current_scene = nullptr;

    void Scene_manager::register_scene(std::shared_ptr<Scene> new_scene) {
        scenes[new_scene->get_name()] = new_scene;
    }

    bool Scene_manager::switch_scene(const std::string& name) {
        auto it = scenes.find(name);
        if (it != scenes.end()) {
            if (current_scene)
                current_scene->on_unload();
            current_scene = it->second;
            current_scene->on_load();
            return true;
        }
        return false;
    }

    void Scene_manager::update(float dt) {
        if (current_scene)
            current_scene->on_update(dt);
    }

    void Scene_manager::render(Render_data& data) {
        if (current_scene)
        {
            current_scene->render_scene(data);
        }
    }

    void Scene_manager::render_ui(Render_data& data) {
        if (current_scene)
            current_scene->render_ui(data);
    }

}
