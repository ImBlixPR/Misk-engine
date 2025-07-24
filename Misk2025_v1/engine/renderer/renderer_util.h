#pragma once
#include "render_data.h"
#include "core/Log.h"
#include <glm/glm.hpp>




namespace renderer_util {
    inline void log_render_data(const Render_data& data) {
        MK_CORE_INFO("=== Logging Render_data ===");

        // Log 2D render items
        MK_CORE_INFO("RenderItems2D Count: {}", data.renderItems2D.size());
        //for (size_t i = 0; i < data.renderItems2D.size(); ++i) {
        //    const auto& item = data.renderItems2D[i];
        //    MK_CORE_INFO("  [RenderItems2D #{}] TextureIndex: {}, TintRGB: ({:.2f}, {:.2f}, {:.2f})",
        //        i, item.textureIndex, item.colorTintR, item.colorTintG, item.colorTintB);
        //}

        // Log high resolution 2D render items
        MK_CORE_INFO("RenderItems2DHiRes Count: {}", data.renderItems2DHiRes.size());
        //for (size_t i = 0; i < data.renderItems2DHiRes.size(); ++i) {
        //    const auto& item = data.renderItems2DHiRes[i];
        //    MK_CORE_INFO("  [RenderItems2DHiRes #{}] TextureIndex: {}, TintRGB: ({:.2f}, {:.2f}, {:.2f})",
        //        i, item.textureIndex, item.colorTintR, item.colorTintG, item.colorTintB);
        //}

        // Log 3D render items
        MK_CORE_INFO("RenderItems3D Count: {}", data.renderItems3D.size());
        for (size_t i = 0; i < data.renderItems3D.size(); ++i) {
            const auto& item = data.renderItems3D[i];
            MK_CORE_INFO("  [RenderItems3D #{}] MeshIndex: {}, BaseColorTexIndex: {}, VertexOffset: {}, IndexOffset: {}",
                i, item.meshIndex, item.baseColorTextureIndex, item.vertexOffset, item.indexOffset);
            MK_CORE_INFO("    AABB Min: ({:.2f}, {:.2f}, {:.2f}), AABB Max: ({:.2f}, {:.2f}, {:.2f})",
                item.aabbMinX, item.aabbMinY, item.aabbMinZ,
                item.aabbMaxX, item.aabbMaxY, item.aabbMaxZ);
        }

        // Log camera data
        for (int i = 0; i < 4; ++i) {
            const auto& cam = data.camera_data[i];
            MK_CORE_INFO("Camera[{}]: Viewport = {}x{}, Offset = ({}, {})",
                i, cam.viewportWidth, cam.viewportHeight, cam.viewportOffsetX, cam.viewportOffsetY);
            MK_CORE_INFO("           ClipSpace X: [{}, {}], Y: [{}, {}]",
                cam.clipSpaceXMin, cam.clipSpaceXMax, cam.clipSpaceYMin, cam.clipSpaceYMax);
            MK_CORE_INFO("           Contrast: {:.2f}, ColorMultiplier RGB: ({:.2f}, {:.2f}, {:.2f})",
                cam.contrast, cam.colorMultiplierR, cam.colorMultiplierG, cam.colorMultiplierB);
        }

        MK_CORE_INFO("=== End of Render_data ===");
    }


    inline glm::mat4 calculate_inverse_matrix(const glm::mat4& matrix) {
        return glm::inverse(matrix);
    }


    inline void calculateAABB(const std::vector<Vertex>& vertices, glm::vec3& aabbMin, glm::vec3& aabbMax) {
        if (vertices.empty()) {
            return;
        }
        aabbMin = vertices[0].position;
        aabbMax = vertices[0].position;
        for (const auto& vertex : vertices) {
            aabbMin = glm::min(aabbMin, vertex.position);
            aabbMax = glm::max(aabbMax, vertex.position);
        }
    }


    //void build_render_object_to_items3D(Render_data& renderItems, std::unordered_map<std::string, Renderable_object>& render_object) {
    //    renderItems.renderItems3D.clear();

    //    // MK_CORE_TRACE("Building render items 3D. Total renderable objects: {}", g_renderableObjects.size());

    //    for (const auto& [name, obj] : render_object) {
    //        if (!obj.visible) {
    //            MK_CORE_TRACE("Skipping invisible object with model name: {}", obj.name);
    //            continue;
    //        }

    //        auto models = asset::get_models_list();

    //        Model& model = models[asset::get_model_index_by_name(obj.name)];
    //        if (!model.m_loaded_from_disk) {
    //            MK_CORE_TRACE("Skipping model '{}' - not loaded from disk", model.get_name());
    //            continue;
    //        }

    //        //MK_CORE_TRACE("Processing model '{}' with {} meshes", model.get_name(), model.get_mesh_count());

    //        glm::mat4 modelMatrix = obj.transform.getModelMatrix();
    //        glm::mat4 inverseModelMatrix = renderer_util::calculate_inverse_matrix(modelMatrix);

    //        // Create render items for each mesh in the model
    //        for (uint32_t meshIndex : model.get_mesh_indices()) {
    //            auto meshes = asset::get_mesh_list();
    //            if (meshIndex >= meshes.size()) {
    //                MK_CORE_ERROR("Invalid mesh index: {} (meshes size: {})", meshIndex, meshes.size());
    //                continue;
    //            }

    //            const Mesh& mesh = meshes[meshIndex];

    //            Render_item3D renderItem;
    //            renderItem.modelMatrix = modelMatrix;
    //            renderItem.inverseModelMatrix = inverseModelMatrix;
    //            renderItem.meshIndex = meshIndex;
    //            renderItem.baseColorTextureIndex = obj.baseColorTextureIndex;
    //            renderItem.vertexOffset = mesh.baseVertex;
    //            renderItem.indexOffset = mesh.baseIndex;

    //            // Transform AABB to world space (simplified - just use mesh AABB)
    //            renderItem.aabbMinX = mesh.aabbMin.x;
    //            renderItem.aabbMinY = mesh.aabbMin.y;
    //            renderItem.aabbMinZ = mesh.aabbMin.z;
    //            renderItem.aabbMaxX = mesh.aabbMax.x;
    //            renderItem.aabbMaxY = mesh.aabbMax.y;
    //            renderItem.aabbMaxZ = mesh.aabbMax.z;

    //            renderItems.renderItems3D.push_back(renderItem);
    //            //MK_CORE_TRACE("Added render item for mesh '{}'", mesh.name);
    //        }
    //    }
    //    //MK_CORE_INFO("RenderItems3D Count: {}", renderItems.renderItems3D.size());
    //}

}
