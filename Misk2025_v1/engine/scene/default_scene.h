#pragma once

#include "base_scene.h"

namespace scene {
    class Default_scene : public Scene {
    public:
        virtual ~Default_scene() = default;

        // Core scene interface
        virtual void on_load() override;
        virtual void on_unload() override;
        virtual void on_update(float delta_time) override;
        virtual void render_scene(Render_data& data) override;
        virtual void render_ui(Render_data& data) override;
        virtual std::string get_name() const override { return "Default"; }
    private:
        struct Scene_data
        {
            bool enable_lighting = true;
            float light_time = 0.0f;
        }scene_data;

        void setup_lighting(Render_data& render_data, float time, bool enable_lighting);
    };

}