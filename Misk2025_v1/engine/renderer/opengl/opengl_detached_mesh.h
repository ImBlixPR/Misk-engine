#pragma once


#include <string>
#include <vector>

#include <glad/glad.h>

#include "core/misk_type.h"

struct opengl_detached_mesh
{

private:
    unsigned int VBO = 0;
    unsigned int VAO = 0;
    unsigned int EBO = 0;
    std::string _name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

public:
    int get_vertex_count() {
        return vertices.size();
    }
    int get_index_count() {
        return indices.size();
    }
    int GetVAO() {
        return VAO;
    }
    std::vector<Vertex> get_verices() {
        return vertices;
    }
    void update_vertex_buffer(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {

        this->indices = indices;
        this->vertices = vertices;

        if (vertices.empty() || indices.empty()) {
            return;
        }

        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};
