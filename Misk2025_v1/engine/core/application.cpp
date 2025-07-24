#include "mkpch.h"
#include "application.h"
#include "input.h"
#include "backend.h"
#include "assets/asset_manager.h"
#include "renderer/renderer.h"
#include "renderer/text_blitter.h"
#include "renderer/render_data.h"
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/renderer_util.h"
#include "renderer/render_object.h"
#include "renderer/opengl/opengl_renderer.h"
#include "scene/scene_manager.h"
#include "scene/default_scene.h"
#include "scene/empty_scene.h"

struct application_state {
    bool is_running;
    bool is_suspended;
    int16_t width;
    int16_t height;
    double last_time;
    Render_data render_data;

    // Scene management
    scene::Scene_manager scene_manager;
};

static bool initialized = false;
static application_state app_state;

bool application_create(Application_config* config) {
    if (initialized) {
        MK_CORE_ERROR("application_create call more than once.");
        return false;
    }

    app_state.is_running = true;
    app_state.is_suspended = false;

    Misk::Log::init();
    backend::init(config);

    // Load initial assets
    asset::load_font();

    // Initialize core systems
    input::init();
    renderer::init();

    // Set up camera projection
    app_state.render_data.camera_data[0].viewportWidth = config->start_window_width;
    app_state.render_data.camera_data[0].viewportHeight = config->start_window_height;

    // Perspective camera
    {
        float fov = glm::radians(45.0f);
        float aspect_ratio = static_cast<float>(config->start_window_width) / static_cast<float>(config->start_window_height);
        float near_plane = 0.1f;
        float far_plane = 1000.0f;
        app_state.render_data.camera_data[0].projection = glm::perspective(fov, aspect_ratio, near_plane, far_plane);
    }

    // Orthographic camera
    {
        float left = 0.0f;
        float right = static_cast<float>(config->start_window_width);
        float bottom = static_cast<float>(config->start_window_height);
        float top = 0.0f;
        float near_plane = -1.0f;
        float far_plane = 1.0f;
        app_state.render_data.camera_data[0].orth_projection = glm::ortho(left, right, bottom, top, near_plane, far_plane);
    }

    // Camera init
    app_state.render_data.camera_data[0].camera = Camera(
        glm::vec3(0.0f, 5.0f, 10.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f, 0.0f, 20.0f, 0.1f
    );

    backend::enable_cursor(false);

    MK_CORE_INFO("Misk engine 2025 version {0}", 1);
    MK_CORE_INFO("Last commit: none");
    MK_CORE_INFO("Test bed application");

    initialized = true;
    return true;
}

bool application_run() {
    bool game_loaded = false;

    while (backend::window_is_open()) {

        backend::start_frame();
        backend::update_subsystem();
        backend::handle_subsystem();
        backend::update_engine_stats();


        if (!asset::is_loading_asset_complate()) {
            asset::load_next_items();
            renderer::render_loading_screen();
        }
        else if (!game_loaded) {
            game_loaded = true;
            asset::upload_vertex_data();

            auto default_scene = std::make_shared<scene::Default_scene>();
            auto empty_scene = std::make_shared<scene::Empty_scene>();

            app_state.scene_manager.register_scene(default_scene);
            app_state.scene_manager.register_scene(empty_scene);

            app_state.scene_manager.switch_scene("empty");
        }
        else {
            float delta_time = backend::get_delta_time();



            app_state.scene_manager.update(delta_time);
            app_state.scene_manager.render(app_state.render_data);
            app_state.scene_manager.render_ui(app_state.render_data);

            renderer::render_frame(app_state.render_data);

            if (input::key_pressed(MK_KEY_X)) {
                app_state.scene_manager.switch_scene("empty");
            }
            if (input::key_pressed(MK_KEY_Z)) {
                app_state.scene_manager.switch_scene("Default");
            }
        }
        backend::end_frame();
    }

    return true;
}

