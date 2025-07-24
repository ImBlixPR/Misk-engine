#pragma once
#include <glad/glad.h>
#include <vector>
#include <core/util.h>
#include <iostream>


struct Color_attachment {
    const char* name = "undefined";
    GLuint handle = 0;
    GLenum internalFormat = GL_RGBA;
};
struct Depth_attachment {
    GLuint handle = 0;
    GLenum internalFormat = GL_RGBA;
};

struct GLFrame_buffer {

private:
    const char* name = "undefined";
    GLuint handle = 0;
    GLuint width = 0;
    GLuint height = 0;
    std::vector<Color_attachment> colorAttachments;
    Depth_attachment depthAttachment;

public:

    void create(const char* name, int width, int height) {
        glGenFramebuffers(1, &handle);
        this->name = name;
        this->width = width;
        this->height = height;
    }

    void clean_up() {
        colorAttachments.clear();
        glDeleteFramebuffers(1, &handle);
    }

    void create_attachment(const char* name, GLenum internalFormat) {
        GLenum slot = GL_COLOR_ATTACHMENT0 + colorAttachments.size();
        Color_attachment& colorAttachment = colorAttachments.emplace_back();
        colorAttachment.name = name;
        colorAttachment.internalFormat = internalFormat;
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
        glGenTextures(1, &colorAttachment.handle);
        glBindTexture(GL_TEXTURE_2D, colorAttachment.handle);
        glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, slot, GL_TEXTURE_2D, colorAttachment.handle, 0);
    }

    GLenum get_depth_attachment_type_from_depth_fromat(GLenum internalFormat) {
        switch (internalFormat) {
        case GL_DEPTH24_STENCIL8:
        case GL_DEPTH32F_STENCIL8:
            return GL_DEPTH_STENCIL_ATTACHMENT;
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32:
        case GL_DEPTH_COMPONENT32F:
            return GL_DEPTH_ATTACHMENT;
        default:
            return 0;
        }
    }

    void create_depth_attachment(GLenum internalFormat) {
        depthAttachment.internalFormat = internalFormat;
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
        glGenTextures(1, &depthAttachment.handle);
        glBindTexture(GL_TEXTURE_2D, depthAttachment.handle);
        glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, get_depth_attachment_type_from_depth_fromat(internalFormat), GL_TEXTURE_2D, depthAttachment.handle, 0);
    }

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
    }

    void bind_external_depth_buffer(GLuint textureHandle) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureHandle, 0);
    }

    void UnbindDepthBuffer() {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    }

    void draw_buffers(std::vector<const char*> attachmentNames) {
        std::vector<GLuint> attachments;
        for (const char* attachmentName : attachmentNames) {
            attachments.push_back(get_color_attachment_slot_by_name(attachmentName));
        }
        glDrawBuffers(attachments.size(), &attachments[0]);
    }

    void draw_buffer(const char* attachmentName) {
        for (int i = 0; i < colorAttachments.size(); i++) {
            if (util::Str_cmp(attachmentName, colorAttachments[i].name)) {
                glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
                return;
            }
        }
    }

    void set_viewport() {
        glViewport(0, 0, width, height);
    }

    GLuint get_handle() {
        return handle;
    }

    GLuint get_width() {
        return width;
    }

    GLuint get_height() {
        return height;
    }

    GLuint get_color_attachment_handle_by_name(const char* name) {
        for (int i = 0; i < colorAttachments.size(); i++) {
            if (util::Str_cmp(name, colorAttachments[i].name)) {
                return colorAttachments[i].handle;
            }
        }
        std::cout << "GetColorAttachmentHandleByName() with name '" << name << "' failed. Name does not exist in FrameBuffer '" << this->name << "'\n";
        return GL_NONE;
    }

    GLuint get_depth_attachment_handle() {
        return depthAttachment.handle;
    }

    GLenum get_color_attachment_slot_by_name(const char* name) {
        for (int i = 0; i < colorAttachments.size(); i++) {
            if (util::Str_cmp(name, colorAttachments[i].name)) {
                return GL_COLOR_ATTACHMENT0 + i;
            }
        }
        std::cout << "GetColorAttachmentHandleByName() with name '" << name << "' failed. Name does not exist in FrameBuffer '" << this->name << "'\n";
        return GL_INVALID_VALUE;
    }

    void clear_attachment(const char* attachmentName, float r, float g, float b, float a) {
        for (int i = 0; i < colorAttachments.size(); i++) {
            if (util::Str_cmp(attachmentName, colorAttachments[i].name)) {
                glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
                glClearColor(r, g, b, a);
                glClear(GL_COLOR_BUFFER_BIT);
                glDrawBuffer(GL_NONE);
                return;
            }
        }
    }

    void clear_depth_attachment() {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
};