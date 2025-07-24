#pragma once
#include <glm/glm.hpp>
#include "render_data.h"
#include "renderer_common.h"
#include "types/types.h"
#include "text_blitter.h"



namespace renderer {
	void init(/*API& api*/);
	void hot_reload_shaders();
	void bind_bindless_textures();
	void render_frame(Render_data& render_data);
	void update_ssbos_gpu(Render_data& render_data);
	void draw_text(std::string text, Render_data& render_data, misk::ivec2 location, misk::ivec2 viewport_size, glm::vec2 scal = glm::vec2(2.0f), Alignment alignment = Alignment::CENTERED, Bitmap_font_type font_type = Bitmap_font_type::STANDARD);
	void render_loading_screen();
}