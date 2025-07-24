#pragma once
#include <string>
#include "opengl/opengl_texture.h"
#include "enums.h"


class Texture {

public:

    Texture() = default;
    Texture(std::string fullpath, bool compressed);
    void load();
    void bake();
    //void BakeCMPData(CMP_Texture* cmpTexture);
    int get_width();
    int get_height();
    std::string& get_filename();
    std::string& get_filetype();
    Opengl_texture& get_gl_texture();
    //VulkanTexture& GetVKTexture();

    void set_loading_state(Loading_state loading_state);
    const Loading_state get_loading_state();
    const Baking_state get_baking_state();

    std::string m_full_path = "";
    bool m_compressed = false;

private:
    Opengl_texture glTexture;
    //VulkanTexture vkTexture;
    std::string m_fileName;
    std::string m_fileType;
    int width = 0;
    int height = 0;
    int channelCount = 0;
    Loading_state m_loadingState = Loading_state::AWAITING_LOADING_FROM_DISK;
    Baking_state m_bakingState = Baking_state::AWAITING_BAKE;
};

