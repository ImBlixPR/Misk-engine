#include "mkpch.h"
#include "opengl_shader.h"



#include <glad/glad.h>



std::string read_text_from_file(std::string path) {
    std::ifstream file(path);
    std::string str;
    std::string line;
    while (std::getline(file, line)) {
        str += line + "\n";
    }
    return str;
}

int check_compile_errors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
    return success;
}




bool gl_shader::load(std::string vertexPath, std::string fragmentPath) {

    std::string vertexSource = read_text_from_file("res/shaders/OpenGL/" + vertexPath);
    std::string fragmentSource = read_text_from_file("res/shaders/OpenGL/" + fragmentPath);

    // Check if files were read successfully
    if (vertexSource.empty()) {
        MK_CORE_ERROR("Failed to read vertex shader: {}", vertexPath);
        return false;
    }
    if (fragmentSource.empty()) {
        MK_CORE_ERROR("Failed to read fragment shader: {}", fragmentPath);
        return false;
    }

    const char* vShaderCode = vertexSource.c_str();
    const char* fShaderCode = fragmentSource.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    if (!check_compile_errors(vertex, "VERTEX")) {
        glDeleteShader(vertex);
        return false;
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    if (!check_compile_errors(fragment, "FRAGMENT")) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }

    int tempID = glCreateProgram();
    glAttachShader(tempID, vertex);
    glAttachShader(tempID, fragment);
    glLinkProgram(tempID);

    if (check_compile_errors(tempID, "PROGRAM")) {
        if (m_ID != -1) {
            glDeleteProgram(m_ID);
        }
        m_ID = tempID;
        m_uniforms_locations.clear();

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        //MK_CORE_INFO("Shader compiled successfully: {} + {}", vertexPath, fragmentPath);
        return true;
    }
    else {
        MK_CORE_ERROR("Shader failed to link: {} + {}", vertexPath, fragmentPath);
        glDeleteProgram(tempID);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }
}


void gl_shader::load(std::string vertexPath, std::string fragmentPath, std::string geomPath) {
    std::string vertexSource = read_text_from_file("res/shaders/OpenGL/" + vertexPath);
    std::string fragmentSource = read_text_from_file("res/shaders/OpenGL/" + fragmentPath);
    std::string geometrySource = read_text_from_file("res/shaders/OpenGL/" + geomPath);

    const char* vShaderCode = vertexSource.c_str();
    const char* fShaderCode = fragmentSource.c_str();
    const char* gShaderCode = geometrySource.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");

    unsigned int geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    check_compile_errors(geometry, "GEOMETRY");

    int tempID = glCreateProgram();
    glAttachShader(tempID, vertex);
    glAttachShader(tempID, fragment);
    glAttachShader(tempID, geometry);
    glLinkProgram(tempID);

    if (check_compile_errors(tempID, "PROGRAM")) {
        if (m_ID != -1) {
            glDeleteProgram(m_ID);
        }
        m_uniforms_locations.clear();
        m_ID = tempID;
    }
    else {
        std::cout << "shader failed to compile " << vertexPath << " " << fragmentPath << "\n";
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geometry);
}


void gl_shader::use() {
    glUseProgram(m_ID);
}

void gl_shader::set_bool(const std::string& name, bool value) {
    if (m_uniforms_locations.find(name) == m_uniforms_locations.end())
        m_uniforms_locations[name] = glGetUniformLocation(m_ID, name.c_str());
    glUniform1i(m_uniforms_locations[name], (int)value);
}

void gl_shader::set_int(const std::string& name, int value) {
    if (m_uniforms_locations.find(name) == m_uniforms_locations.end())
        m_uniforms_locations[name] = glGetUniformLocation(m_ID, name.c_str());
    glUniform1i(m_uniforms_locations[name], value);
}

void gl_shader::set_float(const std::string& name, float value) {
    if (m_uniforms_locations.find(name) == m_uniforms_locations.end())
        m_uniforms_locations[name] = glGetUniformLocation(m_ID, name.c_str());
    glUniform1f(m_uniforms_locations[name], value);
}

void gl_shader::set_mat4(const std::string& name, glm::mat4 value) {
    if (m_uniforms_locations.find(name) == m_uniforms_locations.end())
        m_uniforms_locations[name] = glGetUniformLocation(m_ID, name.c_str());
    glUniformMatrix4fv(m_uniforms_locations[name], 1, GL_FALSE, &value[0][0]);
}

void gl_shader::set_vec3(const std::string& name, const glm::vec3& value) {
    if (m_uniforms_locations.find(name) == m_uniforms_locations.end())
        m_uniforms_locations[name] = glGetUniformLocation(m_ID, name.c_str());
    glUniform3fv(m_uniforms_locations[name], 1, &value[0]);
}

void gl_shader::set_vec2(const std::string& name, const glm::vec2& value) {
    if (m_uniforms_locations.find(name) == m_uniforms_locations.end())
        m_uniforms_locations[name] = glGetUniformLocation(m_ID, name.c_str());
    glUniform2fv(m_uniforms_locations[name], 1, &value[0]);
}

void gl_shader::set_vec4(const std::string& name, const glm::vec4& value) {
    if (m_uniforms_locations.find(name) == m_uniforms_locations.end())
        m_uniforms_locations[name] = glGetUniformLocation(m_ID, name.c_str());
    glUniform4fv(m_uniforms_locations[name], 1, &value[0]);
}