#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

struct RenderItem3D {
    mat4 modelMatrix;
    mat4 inverseModelMatrix;
    int meshIndex;
    int baseColorTextureIndex;
    int vertexOffset;
    int indexOffset;
    float aabbMinX;
    float aabbMinY;
    float aabbMinZ;
    float aabbMaxX;
    float aabbMaxY;
    float aabbMaxZ;
};

layout(std430, binding = 1) readonly buffer renderItems {
    RenderItem3D RenderItems[];
};

uniform int instanceDataOffset;
uniform mat4 view;
uniform mat4 projection;

// Output to fragment shader
out vec3 FragPos;        // World space position
out vec3 Normal;         // World space normal
out vec2 TexCoord;       // Texture coordinates

void main()
{
    int index = gl_InstanceID + gl_BaseInstance + instanceDataOffset;
    mat4 model = RenderItems[index].modelMatrix;
    
    // Calculate world space position
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Proper normal transformation using the inverse transpose
    // For uniform scaling, this simplifies to the upper 3x3 of the model matrix
    mat3 normalMatrix = mat3(transpose(model));
    Normal = normalize(normalMatrix * aNormal);
    
    // Pass through texture coordinates
    TexCoord = aTexCoord;
    
    // Calculate final position
    gl_Position = projection * view * vec4(FragPos, 1.0);
}