#pragma once
#include "renderer/render_data.h"
#include "types/types.h"
#include "renderer/text_blitter.h"
#include "renderer/renderer_common.h"
#include <string>

namespace opengl {
	void init_min();

	void hot_load_shaders();
	void bind_bindless_textures();
	void render_frame(Render_data& render_data);
	void update_ssbos_gpu(Render_data& render_data);

	void render_loading_screen(std::vector<Render_item2D>& renderItems);

	void create_player_render_targets(int presentWidth, int presentHeight);

	void draw_text(std::string text, Render_data& render_data, 
		misk::ivec2 location, misk::ivec2 viewport_size, glm::vec2 scal = glm::vec2(2.0f),
		Alignment alignment = Alignment::CENTERED, Bitmap_font_type font_type = Bitmap_font_type::STANDARD);
	void cleanup();



	
}
