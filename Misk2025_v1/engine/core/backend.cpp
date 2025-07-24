#include "mkpch.h"
#include "backend.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/Log.h"
#include "API/opengl/opengl_backend.h"
#include "core/input.h"
#include "assets/asset_manager.h"





namespace backend {

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

	API _api = API::UNDEFINED;
	bool g_is_glfw_inialize = false;
	GLFWmonitor* g_monitor;
	const GLFWvidmode* g_mode;
	std::string g_window_title;
	int g_window_width = 0;
	int g_window_height = 0;
	float g_window_x_pos = 0;
	float g_window_y_pos = 0;
	int g_full_screen_width = 0;
	int g_full_screen_height = 0;
	window_mode g_current_mode = window_mode::UNDEFINED;
	int g_current_window_width = 0;
	int g_current_window_height = 0;
	GLFWwindow* g_window;
	bool g_force_window_close = false;
	float g_last_time = 0.0f;
	float g_timestep = 0.0f;

	struct Engine_stats {
		double fps_last_time = 0.0;
		int fps_frame_count = 0;
		float current_fps = 0.0f;
		float frame_time = 0.0f;
	} engine_stats;

	//=========================================================================================================

	/*
			.g8"""bgd   .g8""8q. `7MM"""Mq.  `7MM"""YMM
		.dP'     `M .dP'    `YM. MM   `MM.   MM    `7
		dM'       ` dM'      `MM MM   ,M9    MM   d
		MM          MM        MM MMmmdM9     MMmmMM
		MM.         MM.      ,MP MM  YM.     MM   Y  ,
		`Mb.     ,' `Mb.    ,dP' MM   `Mb.   MM     ,M
			`"bmmmd'    `"bmmd"' .JMML. .JMM..JMMmmmmMMM

	*/



	void init(Application_config* config)
	{
		
		if (!g_is_glfw_inialize)
		{
			int succes = glfwInit();

			if (!succes)
			{
				MK_CORE_CRITICAL("Could not initialize GLFW");
			}


			//glfwSetErrorCallback(GLFWErrorCallBack);
			g_is_glfw_inialize = true;
		}

		//window resulution in window mode
		//int width = 1080 * 0.75;
		int height = config->start_window_height;
		int width = config->start_window_width;

		//int height = 1080 * 0.75;

		// OpenGL version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		// Core Profile
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

		// Allow forward compatiblity
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		//glfwSetErrorCallback([](int error, const char* description) { 
			//MK_CORE_CRITICAL("GLFW Error ({0}):{1}", std::to_string(error), description);});

		//context setting
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		//glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

				// Resolution and window size
		g_monitor = glfwGetPrimaryMonitor();
		g_mode = glfwGetVideoMode(g_monitor);
		glfwWindowHint(GLFW_RED_BITS, g_mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, g_mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, g_mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, g_mode->refreshRate);
		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		g_full_screen_width = g_mode->width;
		g_full_screen_height = g_mode->height;
		g_window_width = width;
		g_window_height = height;
		g_window_title = config->title;
		g_window_x_pos = config->start_x_pos;
		g_window_y_pos = config->start_y_pos;

		//window
		create_window(window_mode::WINDOW);

		if (g_window == NULL)
		{
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(g_window);

		_api = config->api;

		if (_api == API::OPENGL)
		{
			asset::find_asset_path();
			opengl_backend::init_minimum();
		}
		else
			return;


	
		
		

		glfwShowWindow(backend::get_window_pointer());
	}


	void start_frame()
	{
		glfwPollEvents();
	}

	void end_frame()
	{
		glfwSwapBuffers(g_window);
	}

	void update_subsystem()
	{
		input::update();
		float time = (float)glfwGetTime();
		g_timestep = time - g_last_time;
		g_last_time = time;
	}

	void handle_subsystem()
	{
		input::handle_window();
	}

	const API GetAPI()
	{
		return _api;
	}

	double get_time()
	{
		return glfwGetTime(); // Returns time in seconds as double
	}


	//=========================================================================================================



	/*
		`7MMF'     A     `7MF'`7MMF'`7MN.   `7MF'`7MM"""Yb.     .g8""8q.`7MMF'     A     `7MF'
		  `MA     ,MA     ,V    MM    MMN.    M    MM    `Yb. .dP'    `YM.`MA     ,MA     ,V
		   VM:   ,VVM:   ,V     MM    M YMb   M    MM     `Mb dM'      `MM VM:   ,VVM:   ,V
			MM.  M' MM.  M'     MM    M  `MN. M    MM      MM MM        MM  MM.  M' MM.  M'
			`MM A'  `MM A'      MM    M   `MM.M    MM     ,MP MM.      ,MP  `MM A'  `MM A'
			 :MM;    :MM;       MM    M     YMM    MM    ,dP' `Mb.    ,dP'   :MM;    :MM;
			  VF      VF      .JMML..JML.    YM  .JMMmmmdP'     `"bmmd"'      VF      VF
	*/

	void create_window(const window_mode& mode)
	{
		if (mode == window_mode::FULLSCREEN)
		{
			g_current_window_width = g_full_screen_width;
			g_current_window_height = g_full_screen_width;
			g_window = glfwCreateWindow(g_current_window_width, g_current_window_height,
				g_window_title.c_str(), nullptr, nullptr);
		}
		else if (mode == window_mode::WINDOW)
		{
			g_current_window_width = g_window_width;
			g_current_window_height = g_window_height;
			g_window = glfwCreateWindow(g_current_window_width, g_current_window_height,
				g_window_title.c_str(), nullptr, nullptr);
			glfwSetWindowPos(g_window, (int)(g_full_screen_width / 2 - g_window_width / 2), (int)g_full_screen_height / 2 - g_window_height / 2);
		}
	}
	void clear_window(float r, float g, float b, float a)
	{

		
		

		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT);
		//166, 177, 209
		//glDisable(GL_DEPTH_TEST);
	}
	GLFWwindow* get_window_pointer()
	{
		return g_window;
	}

	bool window_is_open()
	{
		
		return !(glfwWindowShouldClose(g_window) || g_force_window_close);
	}
	void force_window_to_close()
	{
		g_force_window_close = true;
	}
	int get_window_current_width()
	{
		return g_current_window_width;
	}

	int get_window_current_height()
	{
		return g_current_window_height;
	}
	float get_delta_time()
	{
		return g_timestep;
	}
	void enable_cursor(bool enable)
	{
		if (g_window)
		{
			if (enable)
				glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			else
				glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		}
	}
	//=========================================================================================================
	//============================================engine stats=================================================
	void update_engine_stats()
	{
		engine_stats.fps_frame_count++;
		double current_time = backend::get_time();
		double elapsed = current_time - engine_stats.fps_last_time;

		if (elapsed >= 1.0) {
			engine_stats.current_fps = engine_stats.fps_frame_count / elapsed;
			engine_stats.frame_time = elapsed / engine_stats.fps_frame_count;
			engine_stats.fps_frame_count = 0;
			engine_stats.fps_last_time = current_time;
		}
	}
	float get_fps() { return engine_stats.current_fps; }
	float get_frame_time() { return engine_stats.frame_time; }
	//=========================================================================================================
}







