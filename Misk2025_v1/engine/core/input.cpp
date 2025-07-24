#include "mkpch.h"
#include "input.h"

#include <GLFW/glfw3.h>

#include "core/backend.h"
#include "key_code.h"






namespace input
{

    /*
                       ,,                    ,,        ,,                                            ,,  ,,        ,,
                     `7MM                   *MM      `7MM                                            db *MM      `7MM
                       MM                    MM        MM                                                MM        MM
             .P"Ybmmm  MM  ,pW"Wq.   ,6"Yb.  MM,dMMb.  MM  .gP"Ya      `7M'   `MF',6"Yb.  `7Mb,od8 `7MM  MM,dMMb.  MM  .gP"Ya
            :MI  I8    MM 6W'   `Wb 8)   MM  MM    `Mb MM ,M'   Yb       VA   ,V 8)   MM    MM' "'   MM  MM    `Mb MM ,M'   Yb
             WmmmP"    MM 8M     M8  ,pm9MM  MM     M8 MM 8M""""""        VA ,V   ,pm9MM    MM       MM  MM     M8 MM 8M""""""
            8M         MM YA.   ,A9 8M   MM  MM.   ,M9 MM YM.    ,         VVV   8M   MM    MM       MM  MM.   ,M9 MM YM.    ,
             YMMMMMb .JMML.`Ybmd9'  `Moo9^Yo.P^YbmdP'.JMML.`Mbmmd'          W    `Moo9^Yo..JMML.   .JMML.P^YbmdP'.JMML.`Mbmmd'
            6'     dP
            Ybmmmd'
    */
    GLFWwindow* g_window;
    //mouse bind
    double g_mouse_offset_x = 0;
    double g_mouse_offset_y = 0;
    
    double g_mouse_x= 0;
    double g_mouse_y= 0;
    int g_scroll_wheel_y_offset  = 0;
    bool g_mouse_wheel_up = false;
    bool g_mouse_wheel_down = false;
    bool g_mouse_button_pressed[8] = { false };
    bool g_mouse_button_down[8] = { false };
    bool g_mouse_button_down_last_frame[8] = { false };
    bool g_mouse_button_released[8] = { false };
    //keyborad bind
    bool g_key_pressed[372];
    bool g_key_down[372];
    bool g_key_down_last_frame[372];
    //call back function decloration
    void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset);


    //=================================================================================================

    void init()
    {
        double x, y;
        g_window = backend::get_window_pointer();
        glfwSetScrollCallback(g_window, scroll_callback);
        glfwGetCursorPos(g_window, &x, &y);
        //DisableCursor();
        g_mouse_offset_x = x;
        g_mouse_offset_y = y;
        g_mouse_x = x;
        g_mouse_y = y;
    }
    void update()
    {
        double x, y;
        glfwGetCursorPos(g_window, &x, &y);
        g_mouse_offset_x = x - g_mouse_x;
        g_mouse_offset_y = -1.0*(y - g_mouse_y);
        g_mouse_x = x;
        g_mouse_y = y;
        //get key event using glfw
        for (int i = 32; i < 349; i++) {
            
            //if (g_key_pressed[i]) MK_CORE_TRACE("key pressed:{}", i);

            // to spam the key
            // down
            if (glfwGetKey(g_window, i) == GLFW_PRESS)
                g_key_down[i] = true;
            else
                g_key_down[i] = false;

            // to trigger the key once 
            // press
            if (g_key_down[i] && !g_key_down_last_frame[i])
                g_key_pressed[i] = true;
            else
                g_key_pressed[i] = false;
            g_key_down_last_frame[i] = g_key_down[i];
        }


        // Mouse button handling
        for (int i = 0; i < 8; i++) {
            // Current state
            bool isDown = glfwGetMouseButton(g_window, i) == GLFW_PRESS;

            // Button just pressed this frame (down now, but not last frame)
            g_mouse_button_pressed[i] = isDown && !g_mouse_button_down_last_frame[i];

            // Button just released this frame (not down now, but was last frame)
            g_mouse_button_released[i] = !isDown && g_mouse_button_down_last_frame[i];

            // Button is currently down
            g_mouse_button_down[i] = isDown;

            // Store current state for next frame
            g_mouse_button_down_last_frame[i] = isDown;
        }
    }

    /////////////////////////////
//                         //
//      Keybord bind      //
    bool key_pressed(unsigned int keycode)
    {
        return g_key_pressed[keycode];
    }

    bool key_down(unsigned int keycode)
    {
        return g_key_down[keycode];
    }

    // Function implementations
    bool mouse_button_pressed(int button)
    {
        if (button < 0 || button >= 8)
            return false;
        return g_mouse_button_pressed[button];
    }

    bool mouse_button_down(int button)
    {
        if (button < 0 || button >= 8)
            return false;
        return g_mouse_button_down[button];
    }

    bool mouse_button_released(int button)
    {
        if (button < 0 || button >= 8)
            return false;
        return g_mouse_button_released[button];
    }

    float get_mouse_offset_x() {
        return (float)g_mouse_offset_x;
    }

    float get_mouse_offset_y() {
        return (float)g_mouse_offset_y;
    }

    double get_mouse_x()
    {
        return g_mouse_x;
    }

    double get_mouse_y()
    {
        return g_mouse_y;
    }

    glm::vec2 get_mouse_pos()
    {
        return glm::vec2(get_mouse_x(), get_mouse_y());
    }


    void handle_window()
    {
        if (input::key_pressed(MK_KEY_ESCAPE))
        {
            backend::force_window_to_close();
        }
    }





    /////////////////////////
    //                     //
    //      Callbacks      //
    //!because we dont want to do anything to window and xOffset we pass it as void because the 
    //!call back function need to pass this two parmater
    void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset) {
        g_scroll_wheel_y_offset = (int)yoffset;
    }
}
