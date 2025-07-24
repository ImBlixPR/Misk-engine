#include "mkpch.h"
#include "texture.h"
#include "core/util.h"
#include "core/backend.h"

Texture::Texture(std::string fullpath, bool compressed) {
    m_compressed = compressed;
    m_full_path = fullpath;
    m_fileName = util::get_filename(m_full_path);
    m_fileType = util::get_file_info(m_full_path).ext;
}

void Texture::load() {
    if (backend::GetAPI() == API::OPENGL) {
        glTexture.load(m_full_path, m_compressed);
    }
    else if (backend::GetAPI() == API::VULKAN) {
        //vkTexture.Load(m_fullPath);
    }
    m_loadingState = Loading_state::LOADING_COMPLETE;
    return;
}

void Texture::bake() {
    if (m_bakingState == Baking_state::AWAITING_BAKE) {
        if (backend::GetAPI() == API::OPENGL) {
            glTexture.bake();
        }
        if (backend::GetAPI() == API::VULKAN) {
            //vkTexture.Bake();
        }
    }
    m_bakingState = Baking_state::BAKE_COMPLETE;
}

//void Texture::BakeCMPData(CMP_Texture* cmpTexture) {
//    if (m_bakingState == BakingState::AWAITING_BAKE) {
//        if (BackEnd::GetAPI() == API::OPENGL) {
//            glTexture.BakeCMPData(cmpTexture);
//        }
//        if (BackEnd::GetAPI() == API::VULKAN) {
//            //vkTexture.BakeCMPData(cmpTexture);
//        }
//    }
//    m_bakingState = BakingState::BAKE_COMPLETE;
//}


int Texture::get_width() {
    if (backend::GetAPI() == API::OPENGL) {
        return glTexture.get_width();
    }
    else if (backend::GetAPI() == API::VULKAN) {
        //return vkTexture.GetWidth();
    }
}

int Texture::get_height() {
    if (backend::GetAPI() == API::OPENGL) {
        return glTexture.get_height();
    }
    else if (backend::GetAPI() == API::VULKAN) {
        //return vkTexture.GetHeight();
    }
}

std::string& Texture::get_filename() {
    return m_fileName;
    /*if (BackEnd::GetAPI() == API::OPENGL) {
        return glTexture.GetFilename();
    }
    else if (BackEnd::GetAPI() == API::VULKAN) {
        return vkTexture.GetFilename();
    }*/
}

std::string& Texture::get_filetype() {
    return m_fileType;
    /*
    if (BackEnd::GetAPI() == API::OPENGL) {
        return glTexture.GetFiletype();
    }
    else if (BackEnd::GetAPI() == API::VULKAN) {
        return vkTexture.GetFiletype();
    }*/
}

Opengl_texture& Texture::get_gl_texture() {
    return glTexture;
}

//VulkanTexture& Texture::GetVKTexture() {
//    return vkTexture;
//}

void Texture::set_loading_state(Loading_state loadingState) {
    m_loadingState = loadingState;
}

const Loading_state Texture::get_loading_state() {
    return m_loadingState;
}

const Baking_state Texture::get_baking_state() {
    return m_bakingState;
}
