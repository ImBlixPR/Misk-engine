#include "mkpch.h"
#include "default_scene.h"
#include "core/input.h"
#include "renderer/renderer.h"
#include "core/backend.h"
#include <glm/glm.hpp>
#include "empty_scene.h"



namespace scene {
	void Default_scene::on_load()
	{
		// Initialize scene data
		scene_data.enable_lighting = true;

		//add renderable object to the scene
		
		Transform transform;
		transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
		transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		transform.scale = glm::vec3(0.08f, 0.08f, 0.08f);  // Scale down Sponza
		this->add_scene_3d_object("sponza.obj", transform);

		transform.scale = glm::vec3(5.0f, 5.0f, 5.0f);  // Scale up monkey
		this->add_scene_3d_object("monkey.obj", transform);


        // Offset forward from the camera so object is visible
        transform.position = glm::vec3(30.0f,  40.0f, 7.0f);

        transform.rotation = glm::vec3(0.0f); // Optionally face camera
        transform.scale = glm::vec3(10.0f);    // Monkey scale

        //this->add_scene_3d_object("Cube", transform);
	}
	void Default_scene::on_unload()
	{
        this->scene_3d_objects.clear();
	}
	void Default_scene::on_update(float delta_time)
	{
		// Update light animation time
		scene_data.light_time += delta_time;

		// === LIGHTING CONTROLS ===
		// Toggle lighting with L key
		if (input::key_pressed(MK_KEY_L)) {
			scene_data.enable_lighting = !scene_data.enable_lighting;
			MK_INFO("Lighting {}", scene_data.enable_lighting ? "enabled" : "disabled");
		}

		// === DEBUG CONTROLS ===
		if (input::key_pressed(MK_KEY_Y)) {
			asset::log_data();
			// Note: You'll need to pass render_data to this function somehow
			// renderer_util::log_render_data(render_data);
		}

		if (input::key_pressed(MK_KEY_P)) {
			renderer::hot_reload_shaders();
		}

		// Add lighting debug info
		if (input::key_pressed(MK_KEY_T)) {
			// Note: You'll need access to render_data for this
			// This might need to be moved to render_world method
		}
	}
	void Default_scene::render_scene(Render_data& data)
	{
        data.camera_data[0].camera.free_camera(backend::get_delta_time());
        data.camera_data[0].camera.mouse_control(input::get_mouse_offset_x(), input::get_mouse_offset_y());
        data.camera_data[0].view = data.camera_data[0].camera.calculate_view_matrix();

        if (input::key_pressed(MK_KEY_Q))
        {
            Transform transform;
            glm::vec3 camPos = data.camera_data[0].camera.get_camera_position();
            glm::vec3 camDir = glm::normalize(data.camera_data[0].camera.get_front());

            // Offset forward from the camera so object is visible
            transform.position = camPos + camDir * 5.0f;

            transform.rotation = glm::vec3(0.0f); // Optionally face camera
            transform.scale = glm::vec3(5.0f);    // Monkey scale

            this->add_scene_3d_object("Cube", transform);
        }

		data.renderItems3D.clear();
		for (const auto& obj : this->scene_3d_objects) {
			for (const auto& item : obj.items) {
				data.renderItems3D.push_back(item);
			}
		}

        setup_lighting(data, scene_data.light_time, scene_data.enable_lighting);

	}
	void Default_scene::render_ui(Render_data& data)
	{
        // Clear previous UI elements
        data.renderItems2D.clear();

        // Build UI text
        glm::vec3 camera_position = data.camera_data[0].camera.get_camera_position();
        std::string text = "Misk Engine\n";
        text += "Current scene: " + get_name() + "\n";
        text += "WASD: Move, Arrow Keys: Look Around\n";
        text += "Space/Shift: Up/Down, R: Reset Camera\n";
        text += "L: Toggle Lighting (" + std::string(scene_data.enable_lighting ? "ON" : "OFF") + ")\n";
        text += "T: Log Camera & Light Info\n";
        text += "Y: Log Debug Info\n";
        text += "Camera: (" + std::to_string(camera_position.x) + ", " +
            std::to_string(camera_position.y) + ", " +
            std::to_string(camera_position.z) + ")\n";
        text += "Lights: " + std::to_string(data.render_light_source.size()) + "\n";
        text += "RenderItems3D Count: " + std::to_string(data.renderItems3D.size()) + "\n";
        text += "FPS: " + std::to_string(static_cast<int>(backend::get_fps())) + "\n";

        int viewportWidth = static_cast<int>(data.camera_data[0].viewportWidth);
        int viewportHeight = static_cast<int>(data.camera_data[0].viewportHeight);
        misk::ivec2 viewport_size(viewportWidth, viewportHeight);
        misk::ivec2 center(14, 795);

        renderer::draw_text(text, data, center, viewport_size, glm::vec2(1.0f), Alignment::TOP_LEFT);

        if (input::key_pressed(MK_KEY_R)) {
            MK_INFO("x = {}, y = {}", center.x, center.y);
        }
	}



    void Default_scene::setup_lighting(Render_data& render_data, float time, bool enable_lighting) {
        render_data.render_light_source.clear();

        if (!enable_lighting) {
            Light_source dimLight;
            dimLight.position = glm::vec3(0.0f, 10.0f, 0.0f);
            dimLight.color = glm::vec3(0.1f);
            dimLight.intensity = 1.0f;
            dimLight.constant = 1.0f;
            render_data.render_light_source.push_back(dimLight);
            return;
        }

        // === 1. Sun Light ===
        render_data.render_light_source.push_back({
            glm::vec3(sin(time * 0.5f) * 20.0f, 15.0f + sin(time * 0.3f) * 5.0f, cos(time * 0.5f) * 20.0f),
            glm::vec3(1.0f, 0.95f, 0.8f),
            1.5f, 1.0f, 0.045f, 0.0075f, {0.0f, 0.0f}
            });

        // === 2. Moving Warm Light ===
        render_data.render_light_source.push_back({
            glm::vec3(sin(time * 2.0f) * 8.0f, 4.0f + sin(time * 3.0f) * 2.0f, cos(time * 2.0f) * 8.0f),
            glm::vec3(1.0f, 0.5f, 0.4f),
            0.9f, 1.0f, 0.22f, 0.20f, {0.0f, 0.0f}
            });

        // === 3. Static Blue Accent ===
        render_data.render_light_source.push_back({
            glm::vec3(-10.0f, 8.0f, 5.0f),
            glm::vec3(0.2f, 0.4f, 1.0f),
            0.6f, 1.0f, 0.14f, 0.07f, {0.0f, 0.0f}
            });

        // === 4. Right Fill ===
        render_data.render_light_source.push_back({
            glm::vec3(10.0f, 4.0f, 5.0f),
            glm::vec3(0.8f, 0.8f, 1.0f),
            0.5f, 1.0f, 0.14f, 0.07f, {0.0f, 0.0f}
            });

        // === 5. Back Light ===
        render_data.render_light_source.push_back({
            glm::vec3(0.0f, 5.0f, -10.0f),
            glm::vec3(1.0f, 1.0f, 0.9f),
            0.7f, 1.0f, 0.14f, 0.09f, {0.0f, 0.0f}
            });

        // === 6. Overhead Light ===
        render_data.render_light_source.push_back({
            glm::vec3(0.0f, 20.0f, 0.0f),
            glm::vec3(1.0f),
            1.0f, 1.0f, 0.07f, 0.017f, {0.0f, 0.0f}
            });

        // === 7. Moving Cyan Light ===
        render_data.render_light_source.push_back({
            glm::vec3(6.0f * cos(time), 5.0f + sin(time * 0.5f) * 2.0f, 6.0f * sin(time)),
            glm::vec3(0.0f, 1.0f, 1.0f),
            0.7f, 1.0f, 0.10f, 0.05f, {0.0f, 0.0f}
            });

        // === 8. Pulsing Green Light ===
        render_data.render_light_source.push_back({
            glm::vec3(-6.0f * cos(time), 6.0f, -6.0f * sin(time)),
            glm::vec3(0.3f, 1.0f, 0.3f),
            0.5f + 0.3f * sin(time * 2.0f), 1.0f, 0.10f, 0.05f, {0.0f, 0.0f}
            });

        // === 9. Magenta Light from above right ===
        render_data.render_light_source.push_back({
            glm::vec3(10.0f, 15.0f, -5.0f),
            glm::vec3(1.0f, 0.0f, 1.0f),
            0.6f, 1.0f, 0.12f, 0.06f, {0.0f, 0.0f}
            });
    }


}



