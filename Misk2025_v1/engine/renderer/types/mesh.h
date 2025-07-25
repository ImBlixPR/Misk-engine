#pragma once
#include <string>

#include <glm/glm.hpp>




struct Mesh {

    std::string name = "undefined";

    int32_t GetIndexCount() {
        return indexCount;
    }

    int32_t baseVertex = 0;
    uint32_t baseIndex = 0;
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;

    glm::vec3 aabbMin = glm::vec3(0);
    glm::vec3 aabbMax = glm::vec3(0);
    glm::vec3 extents = glm::vec3(0);
    float boundingSphereRadius = 0;

    

    bool uploadedToGPU = false;
};