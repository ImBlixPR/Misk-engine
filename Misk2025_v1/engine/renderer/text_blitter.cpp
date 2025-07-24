#include "mkpch.h"
#include "text_blitter.h"
#include "core/define.h"
#include <assets/asset_manager.h>
//#include <core/misk_type.h>
#include "renderer_common.h"


namespace text_blitter {

    int _xMargin = 0;
    int _yMargin = 0;
    int _xDebugMargin = 0;
    int _yDebugMargin = 4;
    int _charSpacing = 0;
    int _spaceWidth = 6;
    std::string _charSheet = "•!\"#$%&\'••*+,-./0123456789:;<=>?_ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz";
    std::string _numSheet = "0123456789/";
    int _charCursorIndex = 0;
    float _textTime = 0;
    float _textSpeed = 200.0f;
    float _countdownTimer = 0;
    float _delayTimer = 0;
}

std::vector<Render_item2D> text_blitter::create_text(std::string text, misk::ivec2 location,
    misk::ivec2 viewport_size, Alignment alignment, Bitmap_font_type font_type, glm::vec2 scale)
{
    std::vector<Render_item2D> renderItems;

    if (alignment == Alignment::TOP_LEFT || alignment == Alignment::TOP_RIGHT) {
        location.y -= get_line_height(font_type) * scale.y;
    }
    if (alignment == Alignment::TOP_RIGHT || alignment == Alignment::BOTTOM_RIGHT) {
        location.x -= get_text_size_in_pixels(text, viewport_size, font_type, scale).x;
    }
    if (alignment == Alignment::CENTERED) {
        location.x -= get_text_size_in_pixels(text, viewport_size, font_type, scale).x / 2;
    }


    glm::vec3 color = WHITE;
    int xcursor = location.x;
    int ycursor = location.y;

    for (int i = 0; i < text.length(); i++) {
        char character = text[i];
        if (text[i] == '[' &&
            text[(size_t)i + 1] == 'w' &&
            text[(size_t)i + 2] == ']') {
            i += 2;
            color = WHITE;
            continue;
        }
        else if (text[i] == '[' &&
            text[(size_t)i + 1] == 'g' &&
            text[(size_t)i + 2] == ']') {
            i += 2;
            color = GREEN;
            continue;
        }
        else if (text[i] == '[' &&
            text[(size_t)i + 1] == 'r' &&
            text[(size_t)i + 2] == ']') {
            i += 2;
            color = RED;
            continue;
        }
        else if (text[i] == '[' &&
            text[(size_t)i + 1] == 'l' &&
            text[(size_t)i + 2] == 'g' &&
            text[(size_t)i + 3] == ']') {
            i += 3;
            color = LIGHT_GREEN;
            continue;
        }
        else if (text[i] == '[' &&
            text[(size_t)i + 1] == 'l' &&
            text[(size_t)i + 2] == 'r' &&
            text[(size_t)i + 3] == ']') {
            i += 3;
            color = LIGHT_RED;
            continue;
        }
        else if (character == ' ') {
            xcursor += _spaceWidth;
            continue;
        }
        else if (character == '\n') {
            xcursor = location.x;
            ycursor -= get_line_height(font_type) * scale.y;
            continue;
        }

        std::string textureName = "";
        size_t charPos = std::string::npos;

        if (font_type == Bitmap_font_type::STANDARD) {
            charPos = _charSheet.find(character);
            textureName = "char_" + std::to_string(charPos + 1);
        }
        if (font_type == Bitmap_font_type::AMMO_NUMBERS) {
            charPos = _numSheet.find(character);
            textureName = "num_" + std::to_string(charPos);
        }

        if (charPos == std::string::npos) {
            continue;
        }

        // Get texture index and dimensions
        int textureIndex = asset::get_texture_index_by_name(textureName);
        Texture* texture = asset::get_texture_by_index(textureIndex);
        float texWidth = texture->get_width() * scale.x;
        float texHeight = texture->get_height() * scale.y;
        float width = (1.0f / viewport_size.x) * texWidth;
        float height = (1.0f / viewport_size.y) * texHeight;
        float cursor_X = ((xcursor + (texWidth / 2.0f)) / viewport_size.x) * 2 - 1;
        float cursor_Y = ((ycursor + (texHeight / 2.0f)) / viewport_size.y) * 2 - 1;

        Transform transform;
        transform.position.x = cursor_X;
        transform.position.y = cursor_Y;
        transform.scale = glm::vec3(width, height * -1, 1);



        Render_item2D renderItem;
        renderItem.modelMatrix = transform.getModelMatrix();
        renderItem.textureIndex = textureIndex;
        renderItem.colorTintR = color.r;
        renderItem.colorTintG = color.g;
        renderItem.colorTintB = color.b;
        renderItems.push_back(renderItem);

        xcursor += texWidth + _charSpacing;
    }
    return renderItems;
}

misk::ivec2 text_blitter::get_text_size_in_pixels(std::string text, misk::ivec2 viewport_size, Bitmap_font_type font_type, glm::vec2 scale)
{
    int xcursor = 0;
    int ycursor = 0;
    int maxWidth = 0;
    int maxHeight = 0;

    for (int i = 0; i < text.length(); i++) {
        char character = text[i];
        if (text[i] == '[' &&
            text[(size_t)i + 2] == ']') {
            i += 2;
            continue;
        }
        else if (text[i] == '[' &&
            text[(size_t)i + 1] == 'l' &&
            text[(size_t)i + 3] == ']') {
            i += 3;
            continue;
        }
        else if (character == ' ') {
            xcursor += _spaceWidth;
            maxWidth = std::max(maxWidth, xcursor);
            continue;
        }
        else if (character == '\n') {
            xcursor = 0;
            ycursor += get_line_height(font_type) * scale.y;
            continue;
        }

        std::string textureName = "";
        size_t charPos = std::string::npos;

        if (font_type == Bitmap_font_type::STANDARD) {
            charPos = _charSheet.find(character);
            textureName = "char_" + std::to_string(charPos + 1);
        }
        if (font_type == Bitmap_font_type::AMMO_NUMBERS) {
            charPos = _numSheet.find(character);
            textureName = "num_" + std::to_string(charPos);
        }

        if (charPos == std::string::npos) {
            continue;
        }

        // Get texture index and dimensions
        int textureIndex = asset::get_texture_index_by_name(textureName);
        Texture* texture = asset::get_texture_by_index(textureIndex);
        float texWidth = texture->get_width() * scale.x;
        float texHeight = texture->get_height() * scale.y;
        float width = (1.0f / viewport_size.x) * texWidth;
        float height = (1.0f / viewport_size.y) * texHeight;

        xcursor += texWidth + _charSpacing;
        maxWidth = std::max(maxWidth, xcursor);
    }
    maxWidth = std::max(maxWidth, xcursor);
    return misk::ivec2(maxWidth, ycursor);
}

misk::ivec2 text_blitter::get_character_size(const char* character, Bitmap_font_type font_type)
{
    if (font_type == Bitmap_font_type::STANDARD) {
        static std::unordered_map<const char*, misk::ivec2> standardFontTextureSizes;
        if (standardFontTextureSizes.find(character) == standardFontTextureSizes.end()) {
            int charPos = _charSheet.find(character);
            std::string textureName = "char_" + std::to_string(charPos + 1);
            standardFontTextureSizes[character] = asset::get_texture_size_by_name(textureName.c_str());
        }
        return standardFontTextureSizes[character];
    }
    else if (font_type == Bitmap_font_type::AMMO_NUMBERS) {
        static std::unordered_map<const char*, misk::ivec2> ammoNumbersFontTextureSizes;
        if (ammoNumbersFontTextureSizes.find(character) == ammoNumbersFontTextureSizes.end()) {
            int charPos = _numSheet.find(character);
            std::string textureName = "num_" + std::to_string(charPos);
            ammoNumbersFontTextureSizes[character] = asset::get_texture_size_by_name(textureName.c_str());
        }
        return ammoNumbersFontTextureSizes[character];
    }
    else {
        return misk::ivec2(0, 0);
    }
}

int text_blitter::get_line_height(Bitmap_font_type font_type)
{
    if (font_type == Bitmap_font_type::STANDARD) {
        return 16;
    }
    else if (font_type == Bitmap_font_type::AMMO_NUMBERS) {
        return 34;
    }
    else {
        return 0;
    }
}
