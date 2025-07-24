#include "mkpch.h"
#include "opengl_renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include "renderer/renderer_common.h"
#include "assets/asset_manager.h"
#include "opengl_shader.h"

#include "API/opengl/opengl_backend.h"
#include "opengl_frame_buffer.h"
#include "types/camera.h"
#include "core/backend.h"



#define SCALE_RATIO 2


namespace opengl
{


    //gloable varible

    struct Frame_buffers {

        GLFrame_buffer present;
        GLFrame_buffer gBuffer;
        GLFrame_buffer loading_screen;

    } g_frame_buffers;

    struct shaders
    {
        gl_shader ui_shader;
        gl_shader forword_shader;
    }g_shaders;


    struct shape_mesh
    {

    }g_shapeMesh;

    struct SSBOs {


        GLuint samplers = 0;
        GLuint render_items2D = 0;
        GLuint render_items3D = 0;
        GLuint render_light_source = 0;
        GLuint camera_data = 0;

    } g_ssbos;

    

    void render_ui(std::vector<Render_item2D>& renderItems, GLFrame_buffer& frameBuffer, bool clearScreen);
    void geometry_render_pass(Render_data& renderItems, Camera_data camera_data, GLFrame_buffer& frameBuffer, bool clearScreen);
    

    void blit_frame_buffer(GLFrame_buffer* src, GLFrame_buffer* dst, const char* srcName, const char* dstName, GLbitfield mask, GLenum filter);

    void init_min()
    {
        hot_load_shaders();


        //shader storage buffer obkject
        glCreateBuffers(1, &g_ssbos.render_items2D);
        glNamedBufferStorage(g_ssbos.render_items2D, MAX_RENDER_OBJECTS_2D * sizeof(Render_item2D), NULL, GL_DYNAMIC_STORAGE_BIT);

        glCreateBuffers(1, &g_ssbos.render_items3D);  // Create 3D buffer
        glNamedBufferStorage(g_ssbos.render_items3D, MAX_RENDER_OBJECTS_3D * sizeof(Render_item3D), NULL, GL_DYNAMIC_STORAGE_BIT);

        glCreateBuffers(1, &g_ssbos.render_light_source);
        glNamedBufferStorage(g_ssbos.render_light_source, MAX_LIGHTS * sizeof(Light_source), NULL, GL_DYNAMIC_STORAGE_BIT);

        glCreateBuffers(1, &g_ssbos.camera_data);
        glNamedBufferStorage(g_ssbos.camera_data, sizeof(Camera_data) * 4, NULL, GL_DYNAMIC_STORAGE_BIT);

        //init frame buffer
        //g_frame_buffers.loading_screen.create("LoadingScreen", PRESENT_WIDTH * SCALE_RATIO, PRESENT_HEIGHT * SCALE_RATIO);
        //g_frame_buffers.loading_screen.create_attachment("BaseColor", GL_RGBA8);  

        // Create render targets
        create_player_render_targets(PRESENT_WIDTH, PRESENT_HEIGHT);


       
    }
    void hot_load_shaders()
    {
        MK_CORE_TRACE("Hotloading shaders...");

        // Store old ID to check if load was successful
        int old_id = g_shaders.ui_shader.get_program_id();

        g_shaders.ui_shader.load("GL_ui.vert", "GL_ui.frag");
        g_shaders.forword_shader.load("GL_forword.vert", "GL_forword.frag");


        //int new_id = g_shaders.ui_shader.get_program_id();
        //if (new_id != -1 && new_id != old_id) {
        //    MK_CORE_INFO("UI shader loaded successfully, program ID: {}", new_id);
        //}
        //else if (new_id == -1) {
        //    MK_CORE_ERROR("Failed to load UI shader! Program ID is -1");
        //}
        //else {
        //    MK_CORE_WARN("UI shader load returned same ID: {}", new_id);
        //}
    }
    void bind_bindless_textures()
    {
        // Create the samplers SSBO if needed
        if (g_ssbos.samplers == 0) {
            glCreateBuffers(1, &g_ssbos.samplers);
            glNamedBufferStorage(g_ssbos.samplers, TEXTURE_ARRAY_SIZE * sizeof(glm::uvec2), NULL, GL_DYNAMIC_STORAGE_BIT);
        }
        // Get the handles and stash em in a vector
        std::vector<GLuint64> samplers;
        samplers.reserve(asset::get_texture_count());
        for (int i = 0; i < asset::get_texture_count(); i++) {
            samplers.push_back(asset::get_texture_by_index(i)->get_gl_texture().get_bindless_ID());
        }
        // Send to GPU
        glNamedBufferSubData(g_ssbos.samplers, 0, samplers.size() * sizeof(glm::uvec2), &samplers[0]);

    }
    //void render_frame(Render_data& render_data)
    //{

    //	GLFrame_buffer& gBuffer = g_frame_buffers.gBuffer;
    //	GLFrame_buffer& present = g_frame_buffers.present;

    //	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_ssbos.samplers);
    //	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 16, g_ssbos.camera_data);

    //	render_ui(render_data.renderItems2D, g_frame_buffers.gBuffer, false);

    //	
    //}
    void update_ssbos_gpu(Render_data& render_data)
    {
        glNamedBufferSubData(g_ssbos.camera_data,
            0, 4 * sizeof(Camera_data), &render_data.camera_data[0]);

        // Update render items if any
        if (!render_data.renderItems2D.empty()) {
            glNamedBufferSubData(g_ssbos.render_items2D, 0,
                render_data.renderItems2D.size() * sizeof(Render_item2D),
                render_data.renderItems2D.data());
        }

        if (!render_data.renderItems3D.empty()) {
            glNamedBufferSubData(g_ssbos.render_items3D, 0,
                render_data.renderItems3D.size() * sizeof(Render_item3D),
                render_data.renderItems3D.data());
        }

        if (!render_data.render_light_source.empty()) {
            glNamedBufferSubData(g_ssbos.render_light_source, 0,
                render_data.render_light_source.size() * sizeof(Light_source),
                &render_data.render_light_source[0]);
        }
    }

    void render_loading_screen(std::vector<Render_item2D>& renderItems)
    {
        
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_ssbos.samplers);
        // Check OpenGL errors before rendering
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            MK_CORE_ERROR("OpenGL error before rendering: {}", error);
        }

        // === RENDER TO GBUFFER ===
        g_frame_buffers.loading_screen.bind();
        g_frame_buffers.loading_screen.set_viewport();

        // Clear the gBuffer attachments
        g_frame_buffers.loading_screen.clear_attachment("BaseColor", 0.0f, 0.0f, 0.0f, 0.0f);
        g_frame_buffers.loading_screen.clear_attachment("Normal", 0.0f, 0.0f, 0.0f, 0.0f);
        g_frame_buffers.loading_screen.clear_depth_attachment();

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Set draw buffers for gBuffer rendering
        g_frame_buffers.loading_screen.draw_buffers({ "BaseColor", "Normal" });


        //MK_CORE_INFO("Rendering to gBuffer ({}x{})", g_frame_buffers.loading_screen.get_width(), 
            //g_frame_buffers.loading_screen.get_height());
        render_ui(renderItems, g_frame_buffers.loading_screen, false); // true to clear screen

        // Check for errors after UI render
        error = glGetError();
        if (error != GL_NO_ERROR) {
            MK_CORE_ERROR("OpenGL error after UI render: {}", error);
        }


        glBindFramebuffer(GL_READ_FRAMEBUFFER, g_frame_buffers.loading_screen.get_handle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)

        // Read from BaseColor attachment
        glReadBuffer(g_frame_buffers.loading_screen.get_color_attachment_slot_by_name("BaseColor"));
        glDrawBuffer(GL_BACK);

        glBlitFramebuffer(
            0, 0, g_frame_buffers.loading_screen.get_width(), g_frame_buffers.loading_screen.get_height(),  // Source
            0, 0, PRESENT_WIDTH, PRESENT_HEIGHT,              // Destination  
            GL_COLOR_BUFFER_BIT, GL_LINEAR
        );

        // Final error check
        error = glGetError();
        if (error != GL_NO_ERROR) {
            MK_CORE_ERROR("OpenGL error after blit: {}", error);
        }
    }

    void create_player_render_targets(int presentWidth, int presentHeight)
    {
        if (g_frame_buffers.present.get_handle() != 0) {
            g_frame_buffers.present.clean_up();
        }
        if (g_frame_buffers.gBuffer.get_handle() != 0) {
            g_frame_buffers.gBuffer.clean_up();
        }

        g_frame_buffers.present.create("Present", presentWidth, presentHeight);
        g_frame_buffers.present.create_attachment("Color", GL_RGBA8);
        g_frame_buffers.present.create_depth_attachment(GL_DEPTH32F_STENCIL8);

        g_frame_buffers.gBuffer.create("GBuffer", presentWidth * SCALE_RATIO, presentHeight * SCALE_RATIO);
        g_frame_buffers.gBuffer.create_attachment("BaseColor", GL_RGBA8);
        g_frame_buffers.gBuffer.create_attachment("Normal", GL_RGBA16F);
        g_frame_buffers.gBuffer.create_depth_attachment(GL_DEPTH32F_STENCIL8);

        g_frame_buffers.loading_screen.create("LoadingScreen", presentWidth * SCALE_RATIO, presentHeight * SCALE_RATIO);
        g_frame_buffers.loading_screen.create_attachment("BaseColor", GL_RGBA8);
        g_frame_buffers.loading_screen.create_attachment("Normal", GL_RGBA16F);
        g_frame_buffers.loading_screen.create_depth_attachment(GL_DEPTH32F_STENCIL8);
    }





    void render_frame(Render_data& render_data)
    {
        // Update GPU data first
        update_ssbos_gpu(render_data);

        // Bind bindless textures
        bind_bindless_textures();

        GLFrame_buffer& gBuffer = g_frame_buffers.gBuffer;
        GLFrame_buffer& present = g_frame_buffers.present;

        // Bind SSBOs
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_ssbos.samplers);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 16, g_ssbos.camera_data);

        // Check OpenGL errors before rendering
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            MK_CORE_ERROR("OpenGL error before rendering: {}", error);
        }

        // === RENDER TO GBUFFER ===
        gBuffer.bind();
        gBuffer.set_viewport();

        // Clear the gBuffer attachments
        gBuffer.clear_attachment("BaseColor", 0.0f, 0.0f, 0.0f, 0.0f);
        gBuffer.clear_attachment("Normal", 0.0f, 0.0f, 0.0f, 0.0f);
        gBuffer.clear_depth_attachment();

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Set draw buffers for gBuffer rendering
        gBuffer.draw_buffers({ "BaseColor", "Normal" });

        if (!render_data.renderItems3D.empty()) {
            //MK_CORE_INFO("Rendering {} 3D objects", render_data.renderItems3D.size());
            geometry_render_pass(render_data, render_data.camera_data[0], gBuffer, false);
        }
        else {
            MK_CORE_WARN("No 3D objects to render!");
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //MK_CORE_INFO("Rendering to gBuffer ({}x{})", gBuffer.get_width(), gBuffer.get_height());
        render_ui(render_data.renderItems2D, gBuffer, false); // false since we manually cleared


        // Check for errors after UI render
        error = glGetError();
        if (error != GL_NO_ERROR) {
            MK_CORE_ERROR("OpenGL error after UI render: {}", error);
        }

        // === PRESENT TO SCREEN ===
        // Direct blit to screen
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.get_handle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)

        // Read from BaseColor attachment
        glReadBuffer(gBuffer.get_color_attachment_slot_by_name("BaseColor"));
        glDrawBuffer(GL_BACK);

        glBlitFramebuffer(
            0, 0, gBuffer.get_width(), gBuffer.get_height(),  // Source
            0, 0, PRESENT_WIDTH, PRESENT_HEIGHT,              // Destination  
            GL_COLOR_BUFFER_BIT, GL_LINEAR
        );

        // Final error check
        error = glGetError();
        if (error != GL_NO_ERROR) {
            MK_CORE_ERROR("OpenGL error after blit: {}", error);
        }
    }

    void render_ui(std::vector<Render_item2D>& renderItems, GLFrame_buffer& frameBuffer, bool clearScreen) {



        frameBuffer.bind();
        frameBuffer.set_viewport();

        // GL State
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        if (clearScreen) {
            // FIXED: Use correct attachment name
            frameBuffer.clear_attachment("BaseColor", 1.0f, 1.0f, 1.0f, 1.0f);
            if (frameBuffer.get_depth_attachment_handle() != 0) {
                frameBuffer.clear_depth_attachment();
            }
        }

        // Check if shader is valid
        if (!g_shaders.ui_shader.is_valid()) {
            MK_CORE_ERROR("UI shader is not valid!");
            return;
        }

        // Use shader
        g_shaders.ui_shader.use();


        // Update render items
        glNamedBufferSubData(g_ssbos.render_items2D, 0, renderItems.size() * sizeof(Render_item2D), &renderItems[0]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, g_ssbos.render_items2D);


        // Get mesh
        Mesh* mesh = asset::get_quad_mesh();
        if (!mesh) {
            MK_CORE_ERROR("Quad mesh is null!");
            return;
        }



        // Draw
        glBindVertexArray(opengl_backend::get_vertex_dataVAO());
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * mesh->baseIndex),
            renderItems.size(), mesh->baseVertex);

        // Check for draw errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            MK_CORE_ERROR("OpenGL error after draw call: {}", error);
        }
    }

    // Modified geometry_render_pass function
    void geometry_render_pass(Render_data& renderItems, Camera_data camera_data,
        GLFrame_buffer& frameBuffer, bool clearScreen)
    {
        frameBuffer.bind();
        frameBuffer.set_viewport();

        // Set up OpenGL state for 3D rendering
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);  // Enable culling for better performance
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glDisable(GL_BLEND);

        if (clearScreen) {
            frameBuffer.clear_attachment("BaseColor", 0.1f, 0.1f, 0.15f, 1.0f);  // Dark blue background
            if (frameBuffer.get_depth_attachment_handle() != 0) {
                frameBuffer.clear_depth_attachment();
            }
        }

        if (!g_shaders.forword_shader.is_valid()) {
            MK_CORE_ERROR("Forward shader is not valid!");
            return;
        }

        g_shaders.forword_shader.use();
        g_shaders.forword_shader.set_mat4("view", camera_data.view);
        g_shaders.forword_shader.set_mat4("projection", camera_data.projection);

        // === LIGHTING SETUP ===
        // Get camera position from camera data
        glm::vec3 cameraPos = camera_data.camera.get_camera_position();
        g_shaders.forword_shader.set_vec3("viewPos", cameraPos);

        // Set material properties with better values
        g_shaders.forword_shader.set_float("ambient", 1.15f);   // Slightly more ambient
        g_shaders.forword_shader.set_float("diffuse", 1.8f);    // Good diffuse response
        g_shaders.forword_shader.set_float("specular", 1.3f);   // Moderate specular
        g_shaders.forword_shader.set_float("shininess", 64.0f); // Sharper highlights

        // Set base color (you can make this per-object later)
        g_shaders.forword_shader.set_vec3("baseColor", glm::vec3(0.8f, 0.8f, 0.8f));
        // === END LIGHTING SETUP ===

        // Create SSBO if it doesn't exist
        if (g_ssbos.render_items3D == 0) {
            glCreateBuffers(1, &g_ssbos.render_items3D);
            glNamedBufferStorage(g_ssbos.render_items3D, MAX_RENDER_OBJECTS_3D * sizeof(Render_item3D), NULL, GL_DYNAMIC_STORAGE_BIT);
        }

        // Group render items by mesh
        std::unordered_map<int, std::vector<int>> meshGroups;
        for (int i = 0; i < renderItems.renderItems3D.size(); i++) {
            meshGroups[renderItems.renderItems3D[i].meshIndex].push_back(i);
        }

        // Draw each mesh group
        for (const auto& [meshIndex, instanceIndices] : meshGroups) {
            Mesh* mesh = asset::get_mesh_by_index(meshIndex);
            if (!mesh) {
                MK_CORE_ERROR("Mesh with index {} is null!", meshIndex);
                continue;
            }

            // Create a temporary buffer with only the items for this mesh
            std::vector<Render_item3D> meshRenderItems;
            meshRenderItems.reserve(instanceIndices.size());

            for (int idx : instanceIndices) {
                meshRenderItems.push_back(renderItems.renderItems3D[idx]);
            }

            // Upload only the items for this mesh to the SSBO
            glNamedBufferSubData(g_ssbos.render_items3D, 0,
                meshRenderItems.size() * sizeof(Render_item3D),
                meshRenderItems.data());

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_ssbos.render_items3D);

            // Now instanceDataOffset can be 0 since we repacked the data
            g_shaders.forword_shader.set_int("instanceDataOffset", 0);

            // Bind light sources
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, g_ssbos.render_light_source);
            g_shaders.forword_shader.set_int("numLights", static_cast<int>(renderItems.render_light_source.size()));

            glBindVertexArray(opengl_backend::get_vertex_dataVAO());
            glDrawElementsInstancedBaseVertex(
                GL_TRIANGLES,
                mesh->indexCount,
                GL_UNSIGNED_INT,
                (void*)(sizeof(unsigned int) * mesh->baseIndex),
                static_cast<GLsizei>(instanceIndices.size()),
                mesh->baseVertex
            );

            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                MK_CORE_ERROR("OpenGL error after 3D draw call: {}", error);
            }
        }
    }

    void blit_frame_buffer(GLFrame_buffer* src, GLFrame_buffer* dst, const char* srcName, const char* dstName, GLbitfield mask, GLenum filter) {
        GLint srcHandle = 0;
        GLint dstHandle = 0; 
        GLint srcWidth = backend::get_window_current_width();
        GLint srcHeight = backend::get_window_current_height();
        GLint dstWidth = backend::get_window_current_width();
        GLint dstHeight = backend::get_window_current_height();
        GLenum srcSlot = GL_BACK;
        GLenum dstSlot = GL_BACK;
        if (src) {
            srcHandle = src->get_handle();
            srcWidth = src->get_width();
            srcHeight = src->get_height();
            srcSlot = src->get_color_attachment_slot_by_name(srcName);
        }
        if (dst) {
            dstHandle = dst->get_handle();
            dstWidth = dst->get_width();
            dstHeight = dst->get_height();
            dstSlot = dst->get_color_attachment_slot_by_name(dstName);
        }

        if (dst == nullptr || dstHandle == 0) {
            MK_CORE_INFO("Blitting {} to main screen", srcName);
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, srcHandle);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstHandle);
        glReadBuffer(srcSlot);
        glDrawBuffer(dstSlot);;
        glBlitFramebuffer(0, 0, srcWidth, srcHeight, 0, 0, dstWidth, dstHeight, mask, filter);

   

    }

    void draw_text(std::string text, Render_data& render_data, misk::ivec2 location, misk::ivec2 viewport_size, glm::vec2 scal, Alignment alignment, Bitmap_font_type font_type) {

        std::vector<Render_item2D> text_items = text_blitter::create_text(
            text, location, viewport_size, alignment, font_type, scal);

        render_data.renderItems2D.insert(
            render_data.renderItems2D.end(), text_items.begin(), text_items.end());
    }

    void cleanup() {


        // Clean up SSBOs
        if (g_ssbos.render_items2D != 0) {
            glDeleteBuffers(1, &g_ssbos.render_items2D);
            g_ssbos.render_items2D = 0;
        }
        if (g_ssbos.render_items3D != 0) {
            glDeleteBuffers(1, &g_ssbos.render_items3D);
            g_ssbos.render_items3D = 0;
        }
        if (g_ssbos.camera_data != 0) {
            glDeleteBuffers(1, &g_ssbos.camera_data);
            g_ssbos.camera_data = 0;
        }
        if (g_ssbos.samplers != 0) {
            glDeleteBuffers(1, &g_ssbos.samplers);
            g_ssbos.samplers = 0;
        }
        if (g_ssbos.render_light_source != 0) {
            glDeleteBuffers(1, &g_ssbos.render_light_source);
            g_ssbos.render_light_source = 0;
        }

        // Clean up framebuffers
        g_frame_buffers.present.clean_up();
        g_frame_buffers.gBuffer.clean_up();
    }

}
