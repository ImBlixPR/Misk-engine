// scene_base.h
#pragma once
#include <string>

#include "renderer/renderer_common.h"
#include "renderer/types/mesh.h"
#include "renderer/types/model.h"
#include "renderer/render_data.h"
#include "assets/asset_manager.h"
#include "renderer/renderer_util.h"

struct Scene_3d_object {
    std::string name = "undefine";
    std::vector<Render_item3D> items;
    Transform transfom;

    Scene_3d_object() = default;
    // Constructor
    Scene_3d_object(std::vector<Render_item3D>& item_3d, Transform transform_item, std::string item_name)
        : name(item_name), items(item_3d), transfom(transform_item)
    {
    }

    // Copy constructor
    Scene_3d_object(const Scene_3d_object& other)
        : name(other.name), items(other.items), transfom(other.transfom)
    {
    }

    // Copy assignment operator
    Scene_3d_object& operator=(const Scene_3d_object& other) {
        if (this != &other) {
            name = other.name;
            items = other.items;
            transfom = other.transfom;
        }
        return *this;
    }

    ~Scene_3d_object() {
    }
};


namespace scene {

    class Scene {
    public:
        virtual ~Scene() = default;

        // Core scene interface
        virtual void on_load() = 0;
        virtual void on_unload() = 0;
        virtual void on_update(float delta_time) = 0;
        virtual void render_scene(Render_data& data) = 0;
        virtual void render_ui(Render_data& data) = 0;
        virtual std::string get_name() const = 0;

    protected:
        std::vector<Scene_3d_object> scene_3d_objects;

        bool add_scene_3d_object(std::string name, Transform init_transform)
        {
            Scene_3d_object scene_object;
            scene_object.name = name;
            scene_object.transfom = init_transform;

            auto models = asset::get_models_list();

            Model& model = models[asset::get_model_index_by_name(name)];
            if (!model.m_loaded_from_disk) {
                MK_CORE_TRACE("Skipping model '{}' - not loaded from disk", model.get_name());
                return false;
            }

            //MK_CORE_TRACE("Processing model '{}' with {} meshes", model.get_name(), model.get_mesh_count());

            glm::mat4 modelMatrix = init_transform.getModelMatrix();
            glm::mat4 inverseModelMatrix = renderer_util::calculate_inverse_matrix(modelMatrix);

            // Create render items for each mesh in the model
            for (uint32_t meshIndex : model.get_mesh_indices()) {
                auto meshes = asset::get_mesh_list();
                if (meshIndex >= meshes.size()) {
                    MK_CORE_ERROR("Invalid mesh index: {} (meshes size: {})", meshIndex, meshes.size());
                    continue;
                }

                const Mesh& mesh = meshes[meshIndex];

                Render_item3D renderItem;
                renderItem.modelMatrix = modelMatrix;
                renderItem.inverseModelMatrix = inverseModelMatrix;
                renderItem.meshIndex = meshIndex;
                renderItem.baseColorTextureIndex = 0;
                renderItem.vertexOffset = mesh.baseVertex;
                renderItem.indexOffset = mesh.baseIndex;

                // Transform AABB to world space (simplified - just use mesh AABB)
                renderItem.aabbMinX = mesh.aabbMin.x;
                renderItem.aabbMinY = mesh.aabbMin.y;
                renderItem.aabbMinZ = mesh.aabbMin.z;
                renderItem.aabbMaxX = mesh.aabbMax.x;
                renderItem.aabbMaxY = mesh.aabbMax.y;
                renderItem.aabbMaxZ = mesh.aabbMax.z;

                scene_object.items.push_back(renderItem);
                //MK_CORE_TRACE("Added render item for mesh '{}'", mesh.name);
            }

            scene_3d_objects.push_back(scene_object);
            
        }
    };


} // namespace scene
