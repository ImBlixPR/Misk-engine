// scene_manager.h
#pragma once
#include <memory>
#include <unordered_map>
#include "base_scene.h"

namespace scene {

    class Scene_manager {
    public:
        static void register_scene(std::shared_ptr<Scene> new_scene);
        static bool switch_scene(const std::string& name);
        static void update(float dt);
        static void render(Render_data& data);
        static void render_ui(Render_data& data);

    private:
        static std::unordered_map<std::string, std::shared_ptr<Scene>> scenes;
        static std::shared_ptr<Scene> current_scene;
    };

}
