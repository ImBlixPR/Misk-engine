#pragma once


#include <glm/ext/matrix_float4x4.hpp>

#include "types/camera.h"
#include <glm/ext/matrix_transform.hpp>
#define TEXTURE_ARRAY_SIZE 1024
#define MAX_RENDER_OBJECTS_2D 4096
#define MAX_RENDER_OBJECTS_3D 4096
#define MAX_LIGHTS 32

#define PRESENT_WIDTH 1920 * 0.75//832
#define PRESENT_HEIGHT 1080 * 0.75//468

enum Alignment {
    CENTERED,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

struct Vertex {
    Vertex() = default;
    Vertex(glm::vec3 pos) {
        position = pos;
    }
    Vertex(glm::vec3 pos, glm::vec3 norm) {
        position = pos;
        normal = norm;
    }
    Vertex(glm::vec3 _pos, glm::vec3 _norm, glm::vec2 _uv) {
        position = _pos;
        normal = _norm;
        uv = _uv;
    }
    Vertex(glm::vec3 _position, glm::vec3 _normal, glm::vec2 _uv, glm::vec3 _tangent) {
        position = _position;
        normal = _normal;
        uv = _uv;
        tangent = _tangent;
    }
    glm::vec3 position = glm::vec3(0);
    glm::vec3 normal = glm::vec3(0);
    glm::vec2 uv = glm::vec2(0);
    glm::vec3 tangent = glm::vec3(0);
    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};


struct Camera_data {

    Camera camera;
    glm::mat4 projection = glm::mat4(1);
    glm::mat4 orth_projection = glm::mat4(1);
    glm::mat4 projectionInverse = glm::mat4(1);
    glm::mat4 view = glm::mat4(1);
    glm::mat4 viewInverse = glm::mat4(1);

    float viewportWidth = 0;
    float viewportHeight = 0;
    float viewportOffsetX = 0;
    float viewportOffsetY = 0;

    float clipSpaceXMin;
    float clipSpaceXMax;
    float clipSpaceYMin;
    float clipSpaceYMax;

    float contrast;
    float colorMultiplierR;
    float colorMultiplierG;
    float colorMultiplierB;
};

struct Render_item2D {
    glm::mat4 modelMatrix = glm::mat4(1);
    float colorTintR;
    float colorTintG;
    float colorTintB;
    int textureIndex;
};

struct Render_item3D {
    glm::mat4 modelMatrix;
    glm::mat4 inverseModelMatrix;
    int meshIndex;
    int baseColorTextureIndex;
    //int normalMapTextureIndex;
    //int rmaTextureIndex;
    int vertexOffset;
    int indexOffset;
    //int castShadow;
    //int useEmissiveMask;
    //float emissiveColorR;
    //float emissiveColorG;
    //float emissiveColorB;
    float aabbMinX;
    float aabbMinY;
    float aabbMinZ;
    float aabbMaxX;
    float aabbMaxY;
    float aabbMaxZ;
    //float padding0;
    //float padding1;
    //float padding2;
};

struct Light_source {
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        float constant;
        float linear;
        float quadratic;
        float padding[2]; // For alignment
 };


struct Bounding_box {
    glm::vec3 size;
    glm::vec3 offset_from_model_origin;
};

struct WeightedVertex {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 normal = glm::vec3(0);
    glm::vec2 uv = glm::vec2(0);
    glm::vec3 tangent = glm::vec3(0);
    glm::ivec4 boneID = glm::ivec4(0);
    glm::vec4 weight = glm::vec4(0);

    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // Euler angles in radians
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        return model;
    }
};



