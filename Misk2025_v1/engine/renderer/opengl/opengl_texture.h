#pragma once

#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <Compressonator.h>
#include <string>
#include <memory>

struct Opengl_texture {

    Opengl_texture() = default;
    //explicit OpenGLTexture(const std::string filepath);
    GLuint getID();
    GLuint64 get_bindless_ID();
    void bind(unsigned int slot);
    bool load(const std::string filepath, bool compressed);
    bool bake();
    //void BakeCMPData(CMP_Texture* cmpTexture);
    //void UploadToGPU(void* data, CMP_Texture* cmpTexture, int width, int height, int channelCount);
    bool is_baked();
    int get_width();
    int get_height();
    std::string& get_filename();
    std::string& get_filetype();
    void hotload_from_path(const std::string filepath);
    void make_bindless_texture_resident();
    void make_bindless_texture_non_resident();

    GLuint& get_handle_reference();
    void generate_mipmaps();

private:
    GLuint ID;
    GLuint64 bindlessID = 0;
    std::string m_fullPath;
    std::string m_filename;
    std::string m_filetype;
    bool m_compressed = false;

    //unsigned char* _data = nullptr;
    //float* _floatData = nullptr;

    //CompressedTextureData m_compressedTextureData;

    void* m_data = nullptr;

    int _NumOfChannels = 0;
    int _width = 0;
    int _height = 0;
    //bool _baked = false;
};


