#pragma once

#include <glad/glad.h>

//#include "core/misk_type.h"
#include "renderer/renderer_common.h"

namespace opengl_backend
{

	void init_minimum();
    void upload_vertex_data(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    void upload_weighted_vertex_data(std::vector<WeightedVertex>& vertices, std::vector<uint32_t>& indices);
    GLuint get_vertex_dataVAO();
    GLuint get_vertex_dataVBO();
    GLuint get_vertex_dataEBO();
}
