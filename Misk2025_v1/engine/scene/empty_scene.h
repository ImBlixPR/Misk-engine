#pragma once
#include "base_scene.h"


#include "base_scene.h"

namespace scene {
    class Empty_scene : public Scene {
    public:
        virtual ~Empty_scene() = default;

        // Core scene interface
        virtual void on_load() override;
        virtual void on_unload() override;
        virtual void on_update(float delta_time) override;
        virtual void render_scene(Render_data& data) override;
        virtual void render_ui(Render_data& data) override;
        virtual std::string get_name() const override { return "empty"; }
    private:
        struct Scene_data
        {
            bool enable_lighting = true;
            float light_time = 0.0f;
        }scene_data;

        void setup_lighting(Render_data& render_data, float time, bool enable_lighting);
    };

}
