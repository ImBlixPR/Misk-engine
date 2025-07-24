#include "mkpch.h"
#include "render_object.h"
#include "renderer_util.h"
#include "assets/asset_manager.h"

namespace render_object
{


	////gloabel 
 //   std::unordered_map<std::string, Renderable_object> g_renderableObjects;


 //   int create_renderable_object(std::string model_name, int textureIndex) {
 //       if (g_renderableObjects.find(model_name) != g_renderableObjects.end()) {
 //           MK_CORE_WARN("Renderable object with name '{}' already exists", model_name);
 //           return -1;
 //       }
 //       g_renderableObjects[model_name] = Renderable_object(model_name, textureIndex);
 //       return static_cast<int>(g_renderableObjects.size()) - 1;

 //   }

 //   void update_renderable_object(const std::string& model_name, const Transform& transform) {
 //       auto it = g_renderableObjects.find(model_name);
 //       if (it != g_renderableObjects.end()) {
 //           it->second.transform = transform;
 //       }
 //       else {
 //           MK_WARN("Model with name '{}' not found in renderable objects!", model_name);
 //       }
 //   }

 //   void build_render_items3D(Render_data& renderItems) {
 //       renderItems.renderItems3D.clear();

 //      // MK_CORE_TRACE("Building render items 3D. Total renderable objects: {}", g_renderableObjects.size());

 //       for (const auto& [name, obj] : g_renderableObjects) {
 //           if (!obj.visible) {
 //               MK_CORE_TRACE("Skipping invisible object with model name: {}", obj.name);
 //               continue;
 //           }

 //           auto models = asset::get_models_list();

 //           Model& model = models[asset::get_model_index_by_name(obj.name)];
 //           if (!model.m_loaded_from_disk) {
 //               MK_CORE_TRACE("Skipping model '{}' - not loaded from disk", model.get_name());
 //               continue;
 //           }

 //           //MK_CORE_TRACE("Processing model '{}' with {} meshes", model.get_name(), model.get_mesh_count());

 //           glm::mat4 modelMatrix = obj.transform.getModelMatrix();
 //           glm::mat4 inverseModelMatrix = renderer_util::calculate_inverse_matrix(modelMatrix);

 //           // Create render items for each mesh in the model
 //           for (uint32_t meshIndex : model.get_mesh_indices()) {
 //               auto meshes = asset::get_mesh_list();
 //               if (meshIndex >= meshes.size()) {
 //                   MK_CORE_ERROR("Invalid mesh index: {} (meshes size: {})", meshIndex, meshes.size());
 //                   continue;
 //               }

 //               const Mesh& mesh = meshes[meshIndex];

 //               Render_item3D renderItem;
 //               renderItem.modelMatrix = modelMatrix;
 //               renderItem.inverseModelMatrix = inverseModelMatrix;
 //               renderItem.meshIndex = meshIndex;
 //               renderItem.baseColorTextureIndex = obj.baseColorTextureIndex;
 //               renderItem.vertexOffset = mesh.baseVertex;
 //               renderItem.indexOffset = mesh.baseIndex;

 //               // Transform AABB to world space (simplified - just use mesh AABB)
 //               renderItem.aabbMinX = mesh.aabbMin.x;
 //               renderItem.aabbMinY = mesh.aabbMin.y;
 //               renderItem.aabbMinZ = mesh.aabbMin.z;
 //               renderItem.aabbMaxX = mesh.aabbMax.x;
 //               renderItem.aabbMaxY = mesh.aabbMax.y;
 //               renderItem.aabbMaxZ = mesh.aabbMax.z;

 //               renderItems.renderItems3D.push_back(renderItem);
 //               //MK_CORE_TRACE("Added render item for mesh '{}'", mesh.name);
 //           }
 //       }
 //       //MK_CORE_INFO("RenderItems3D Count: {}", renderItems.renderItems3D.size());
 //   }

 //   Renderable_object* get_renderable_object(const std::string& name) {
 //       auto it = g_renderableObjects.find(name);
 //       if (it != g_renderableObjects.end()) {
 //           return &it->second;
 //       }
 //       return nullptr;
 //   }

 //   int get_renderable_object_count() {
 //       return static_cast<int>(g_renderableObjects.size());
 //   }

 //   void clear_up() {

 //   }


}
