#pragma once
#include <vector>
//#include <glm/ext/matrix_float4x4.hpp>
#include "renderer_common.h"
#include "types/camera.h"




struct Render_data {

    std::vector<Render_item2D> renderItems2D;
    std::vector<Render_item2D> renderItems2DHiRes;
    std::vector<Render_item3D> renderItems3D;
    std::vector<Light_source> render_light_source;
    Camera_data camera_data[4];

};