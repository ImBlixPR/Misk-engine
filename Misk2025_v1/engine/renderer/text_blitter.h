#pragma once

#include "renderer_common.h"
#include "types/types.h"

#include <vector>
#include <string>
#include <glm/glm.hpp>

enum Bitmap_font_type {
    STANDARD,
    AMMO_NUMBERS
};

namespace text_blitter {

    std::vector<Render_item2D> create_text(std::string text, misk::ivec2 location,
        misk::ivec2 viewport_size, Alignment alignment, Bitmap_font_type font_type, glm::vec2 scale = glm::vec2(1.0f));
    misk::ivec2 get_text_size_in_pixels(std::string text, misk::ivec2 viewport_size, Bitmap_font_type font_type, glm::vec2 scale = glm::vec2(1.0f));
    misk::ivec2 get_character_size(const char* character, Bitmap_font_type font_type);
    int get_line_height(Bitmap_font_type font_type);

}