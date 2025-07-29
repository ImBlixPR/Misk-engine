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

//temp
#include <GLFW/glfw3.h>

struct application_state {
    bool is_running;
    bool is_suspended;
    int16_t width;
    int16_t height;
    double last_time;
    Render_data render_data;

    // Scene management
    scene::Scene_manager scene_manager;

    // Map Editor with performance controls
    GLFWwindow* map_editor_window;
    bool editor_open = false;

    // Editor timing control
    double editor_last_update = 0.0;
    double editor_update_interval = 1.0 / 30.0; // 60 FPS for editor (can be lower)
    bool editor_needs_redraw = true;

    struct EditorState {
        bool grid_enabled = true;
        int selected_tool = 0;
        float zoom_level = 1.0f;
        // Dirty flag for optimization
        bool needs_update = false;
    } editor_state;
};

static bool initialized = false;
static application_state app_state;

// Optimized editor input handling
void handle_editor_input_optimized(GLFWwindow* editor_window, float delta_time) {
    if (!editor_window || !backend::is_window_focused(editor_window)) return;

    backend::make_window_current(editor_window);
    bool state_changed = false;

    // Editor-specific controls (only when focused)
    if (glfwGetKey(editor_window, GLFW_KEY_G) == GLFW_PRESS) {
        static bool g_pressed = false;
        if (!g_pressed) {
            app_state.editor_state.grid_enabled = !app_state.editor_state.grid_enabled;
            MK_CORE_INFO("Grid toggled: {}", app_state.editor_state.grid_enabled ? "ON" : "OFF");
            state_changed = true;
            g_pressed = true;
        }
    }
    else {
        static bool g_pressed = false;
        g_pressed = false;
    }

    if (glfwGetKey(editor_window, GLFW_KEY_1) == GLFW_PRESS) {
        static bool key1_pressed = false;
        if (!key1_pressed && app_state.editor_state.selected_tool != 0) {
            app_state.editor_state.selected_tool = 0;
            MK_CORE_INFO("Selected tool: Brush");
            state_changed = true;
            key1_pressed = true;
        }
    }
    else {
        static bool key1_pressed = false;
        key1_pressed = false;
    }

    if (glfwGetKey(editor_window, GLFW_KEY_2) == GLFW_PRESS) {
        static bool key2_pressed = false;
        if (!key2_pressed && app_state.editor_state.selected_tool != 1) {
            app_state.editor_state.selected_tool = 1;
            MK_CORE_INFO("Selected tool: Eraser");
            state_changed = true;
            key2_pressed = true;
        }
    }
    else {
        static bool key2_pressed = false;
        key2_pressed = false;
    }

    // Zoom controls (continuous)
    if (glfwGetKey(editor_window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        float old_zoom = app_state.editor_state.zoom_level;
        app_state.editor_state.zoom_level = std::min(5.0f, app_state.editor_state.zoom_level + delta_time * 2.0f);
        if (old_zoom != app_state.editor_state.zoom_level) state_changed = true;
    }

    if (glfwGetKey(editor_window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        float old_zoom = app_state.editor_state.zoom_level;
        app_state.editor_state.zoom_level = std::max(0.1f, app_state.editor_state.zoom_level - delta_time * 2.0f);
        if (old_zoom != app_state.editor_state.zoom_level) state_changed = true;
    }

    // Mark for redraw if state changed
    if (state_changed) {
        app_state.editor_state.needs_update = true;
        app_state.editor_needs_redraw = true;
    }
}


// Optimized editor rendering with conditional updates
void render_map_editor_optimized(GLFWwindow* editor_window, bool force_render = false) {
    if (!editor_window) return;

    double current_time = backend::get_time();

    // Only render if needed and enough time has passed, or forced
    bool should_render = force_render ||
        app_state.editor_needs_redraw ||
        (current_time - app_state.editor_last_update) >= app_state.editor_update_interval;

    if (!should_render) return;

    backend::make_window_current(editor_window);


    // Clear with different color to distinguish from main window
    backend::clear_window_with_context(editor_window, 0.15f, 0.15f, 0.2f, 1.0f);

    // Render editor content
    // - Grid (if enabled)
    if (app_state.editor_state.grid_enabled) {
        // Render grid here
    }

    // - Tools panel
    // - Selected tool indicator
    // - Zoom level indicator

    // Focus indicator
    if (backend::is_window_focused(editor_window)) {
        // Render focus indicator (brighter border, etc.)
    }

    backend::swap_window_buffers(editor_window);

    // Update timing
    app_state.editor_last_update = current_time;
    app_state.editor_needs_redraw = false;
    app_state.editor_state.needs_update = false;
}

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


    // Set up focus callbacks after backend init
    backend::set_window_focus_callbacks();

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

    app_state.map_editor_window = nullptr;

    initialized = true;
    return true;
}

bool application_run() {
    bool game_loaded = false;

    // Performance monitoring
    double main_window_time = 0.0;
    double editor_window_time = 0.0;

    while (backend::window_is_open()) {
        double frame_start = backend::get_time();

        // Handle all window events (minimal cost)
        backend::poll_all_windows();
        backend::update_subsystem();
        backend::handle_subsystem();
        backend::update_engine_stats();

        // Handle map editor toggle (only if main window is focused)
        if (backend::is_window_focused(backend::get_window_pointer()) &&
            input::key_pressed(MK_KEY_F1)) {

            if (!app_state.editor_open) {
                app_state.map_editor_window = backend::create_secondary_window(
                    "Map Editor", 1200, 800, true);

                if (app_state.map_editor_window) {
                    app_state.editor_open = true;
                    app_state.editor_needs_redraw = true;
                    MK_CORE_INFO("Map Editor opened - Running in parallel with main window");
                }
            }
            else {
                backend::destroy_secondary_window(app_state.map_editor_window);
                app_state.map_editor_window = nullptr;
                app_state.editor_open = false;
                MK_CORE_INFO("Map Editor closed");
            }
        }

        if (!asset::is_loading_asset_complate()) {
            asset::load_next_items();
            backend::make_window_current(backend::get_window_pointer());
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
            double main_start = backend::get_time();

            // ===== MAIN WINDOW (ALWAYS FULL SPEED) =====
            backend::make_window_current(backend::get_window_pointer());

            // Update game logic (never throttled)
            app_state.scene_manager.update(delta_time);
            app_state.scene_manager.render(app_state.render_data);
            app_state.scene_manager.render_ui(app_state.render_data);
            renderer::render_frame(app_state.render_data);

            // Handle main window controls (only when focused)
            if (backend::is_window_focused(backend::get_window_pointer())) {
                if (input::key_pressed(MK_KEY_X)) {
                    app_state.scene_manager.switch_scene("empty");
                }
                if (input::key_pressed(MK_KEY_Z)) {
                    app_state.scene_manager.switch_scene("Default");
                }
            }

            main_window_time += backend::get_time() - main_start;

            // ===== EDITOR WINDOW (SMART THROTTLING) =====
            if (app_state.editor_open && backend::is_window_open(app_state.map_editor_window)) {
                double editor_start = backend::get_time();

                // Always handle input (low cost)
                handle_editor_input_optimized(app_state.map_editor_window, delta_time);

                // Render only when needed (saves GPU)
                render_map_editor_optimized(app_state.map_editor_window);

                editor_window_time += backend::get_time() - editor_start;
            }
            else if (app_state.editor_open && !backend::is_window_open(app_state.map_editor_window)) {
                app_state.map_editor_window = nullptr;
                app_state.editor_open = false;
                MK_CORE_INFO("Map Editor closed by user");
            }
        }

        backend::end_frame();

        // Optional: Log performance every second
        static double last_perf_log = 0.0;
        double current_time = backend::get_time();
        if (current_time - last_perf_log > 1.0) {
            MK_CORE_INFO("Performance - Main: {:.2f}ms, Editor: {:.2f}ms",
                main_window_time * 1000.0, editor_window_time * 1000.0);
            main_window_time = editor_window_time = 0.0;
            last_perf_log = current_time;
        }
    }

    if (app_state.editor_open) {
        backend::destroy_secondary_window(app_state.map_editor_window);
    }

    return true;
}

