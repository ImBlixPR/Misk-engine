#pragma once


#include <string>

#include "misk_enum.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/hash.hpp"



struct Application_config
{
    std::string title;
    float start_x_pos = 0.0f;
    float start_y_pos = 0.0f;
    int start_window_width = 720;
    int start_window_height = 720;
    API api = API::OPENGL;
};

struct Buffer_object {
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
};



struct Shape_vertex {
    glm::vec2 Position;
};


struct Texture_data {
    std::string m_filepath;
    std::string m_file_extenction;
    int m_width = 0;
    int m_height = 0;
    int m_num_channels = 0;
    void* m_data = nullptr;
};


//positioning status

struct Position_status
{
    Position_status(glm::vec3 position)
    {
        m_position = position;
    }
    Position_status(glm::vec3 position, float angle)
    {
        m_position = position;
        m_angle = angle;
    }
    Position_status(glm::vec3 position, float angle, glm::vec3 direction)
    {
        m_position = position;
        m_angle = angle;
        m_direction = direction;
    }

    glm::vec3 m_position = glm::vec3(0.0f);
    float m_angle = 0.0f;
    glm::vec3 m_direction = glm::vec3(0.0f, 1.0f, 0.0f);
    bool operator==(const Position_status& other) const {
        return m_position == other.m_position && m_angle == other.m_angle && m_direction == other.m_direction;
    }
};

struct Transform_old {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);
    glm::mat4 to_mat4() {
        glm::mat4 m = glm::translate(glm::mat4(1), position);
        m *= glm::mat4_cast(glm::quat(rotation));
        m = glm::scale(m, scale);
        return m;
    };
    glm::vec3 to_forward_vector() {
        glm::quat q = glm::quat(rotation);
        return glm::normalize(q * glm::vec3(0.0f, 0.0f, 1.0f));
    }
    glm::vec3 to_right_vector() {
        glm::quat q = glm::quat(rotation);
        return glm::normalize(q * glm::vec3(1.0f, 0.0f, 0.0f));
    }
};


struct File_info {
    std::string path;
    std::string name;
    std::string ext;
    std::string dir;
    std::string materialType;
    std::string GetFileNameWithExtension() {
        if (ext.length() > 0) {
            return name + "." + ext;
        }
        else {
            return name;
        }
    }
};

