#pragma once

#include <string>

#include <unordered_map>
#include <glm/glm.hpp>


struct gl_shader {
public:
    bool load(std::string vertexPath, std::string fragmentPath);
    void load(std::string vertexPath, std::string fragmentPath, std::string geomPath);
    void use();
    void set_bool(const std::string& name, bool value);
    void set_int(const std::string& name, int value);
    void set_float(const std::string& name, float value);
    void set_mat4(const std::string& name, glm::mat4 value);
    void set_vec4(const std::string& name, const glm::vec4& value);
    void set_vec3(const std::string& name, const glm::vec3& value);
    void set_vec2(const std::string& name, const glm::vec2& value);
    bool is_valid() const { return m_ID != -1; }
    int get_program_id() const { return m_ID; }
private:
    int m_ID = -1;
    std::unordered_map<std::string, int> m_uniforms_locations;
};
