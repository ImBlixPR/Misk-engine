#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 baseColor;
uniform vec3 viewPos;           // Camera position
uniform int numLights;          // Number of active lights

// Material properties
uniform float ambient;
uniform float diffuse;
uniform float specular;
uniform float shininess;

// Light source structure (must match C++ struct)
struct Light_source {
        vec3 position;
        vec3 color;
        float intensity;
        float constant;
        float linear;
        float quadratic;
        float padding[2]; // For alignment
 };

// Light SSBO
layout(std430, binding = 3) buffer Light_buffer {
    Light_source lights[];
};

vec3 calculatePointLight(Light_source light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Attenuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // Ambient
    vec3 ambientComponent = ambient * light.color * attenuation;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseComponent = diffuse * diff * light.color * attenuation;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specularComponent = specular * spec * light.color;// attenuation;
    return specularComponent;
    return (ambientComponent + diffuseComponent + specularComponent) * light.intensity;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    
    // Calculate lighting for each light source
    for(int i = 0; i < numLights && i < lights.length(); i++)
    {
        result += calculatePointLight(lights[i], norm, viewDir, FragPos);
    }
    
    // Apply base color
    result *= baseColor;
    
    // Add minimal global ambient to prevent pure black
    vec3 globalAmbient = vec3(0.02) * baseColor;
    result += globalAmbient;
    
    // Tone mapping and gamma correction
    result = result / (result + vec3(1.0)); // Simple Reinhard tone mapping
    result = pow(result, vec3(1.0/2.2)); // Gamma correction
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    
}