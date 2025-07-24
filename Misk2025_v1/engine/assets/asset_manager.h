#pragma once

#include <iostream>
#include "misk_export.h"
#include "renderer/texture.h"
#include "types/types.h"
#include "renderer/renderer_common.h"
#include "renderer/types/mesh.h"
#include <vector>
#include "renderer/types/model.h"

//#include "types/MiskTypes.h"

namespace asset
{
	// unsigned int load_texture(std::string filepath);


	// std::string read_text_from_file(std::string path);

    void upload_vertex_data();

    // Textures
    void load_texture(Texture* texture);
    Texture* get_texture_by_name(const std::string& name);
    Texture* get_texture_by_index(const int index);
    int get_texture_count();
    int get_texture_index_by_name(const std::string& filename, bool ignoreWarning = false);
    bool texture_exists(const std::string& name);
    std::vector<Texture>& get_textures();
    void load_font();
    misk::ivec2 get_texture_size_by_name(const char* textureName);

    void find_asset_path();

    //model
    void create_hardcoded_model();
    void load_model_from_file(const std::string& filename);
    std::vector<Model> get_models_list();
    int get_model_index_by_name(const std::string& name);

    //mesh
    int create_mesh(std::string name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, glm::vec3 aabbMin, glm::vec3 aabbMax);
    Mesh* get_quad_mesh();
    Mesh* get_mesh_by_index(int index);
    std::vector<Mesh> get_mesh_list();

    int get_quad_mesh_index();
    int get_up_facing_plane_mesh_index();
    int get_cube_mesh_index();


    void load_next_items();
    bool is_loading_asset_complate();


    //log
    void add_item_to_load_log(std::string item);
    std::vector<std::string>& get_load_log();
    void log_data();
}
