#pragma once

#include "core/misk_enum.h"
#include "misk_export.h"
#include "misk_type.h"

struct GLFWwindow;
namespace backend
{
    //core
     void init(Application_config* config);
     void start_frame();
     void end_frame();
     void update_subsystem();
     void handle_subsystem();
    const API GetAPI();
    double get_time();

    //window
     void create_window(const window_mode& mode);
     void clear_window(float r, float b, float g, float a);
     GLFWwindow* get_window_pointer();
     bool window_is_open();
     void force_window_to_close();
     int get_window_current_width();
     int get_window_current_height();
     float get_delta_time();
     void enable_cursor(bool enable);

     //engine
     void update_engine_stats();
     float get_fps();
     float get_frame_time();
     float get_delta_time();
}