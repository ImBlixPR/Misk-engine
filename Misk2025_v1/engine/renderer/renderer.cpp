#include "mkpch.h"

#include "renderer.h"

#include "opengl/opengl_renderer.h"
#include "assets/asset_manager.h"


namespace renderer
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

	API g_api;


	std::vector<Render_item2D> create_loading_screen_render_items();
	void create_loading_screen_render_items(std::vector<Render_item2D>& render_data);


	void init(/*const API& api*/)
	{
		g_api = API::OPENGL;
		if (g_api == API::OPENGL)
		{
			opengl::init_min();
		}
	}

	void hot_reload_shaders()
	{
		if (g_api == API::OPENGL)
		{
			opengl::hot_load_shaders();
		}
	}

	void bind_bindless_textures() 
	{
		if (g_api == API::OPENGL)
		{
			opengl::bind_bindless_textures();
		}
	}

	void render_frame(Render_data& render_data)
	{


		if (g_api == API::OPENGL)
		{
			
			opengl::render_frame(render_data);
		}
	}
	void update_ssbos_gpu(Render_data& render_data)
	{

		if (g_api == API::OPENGL)
		{
			opengl::update_ssbos_gpu(render_data);
		}
	}

	void draw_text(std::string text, Render_data& render_data, misk::ivec2 location, misk::ivec2 viewport_size, glm::vec2 scal , Alignment alignment , Bitmap_font_type font_type )
	{
		if (g_api == API::OPENGL)
		{
			opengl::draw_text(text, render_data,location, viewport_size, scal , alignment , font_type);
		}
	}

	void render_loading_screen()
	{
		
		std::vector<Render_item2D> render_items = create_loading_screen_render_items();
		//MK_CORE_CRITICAL("item number = {}", render_items.size());
		opengl::render_loading_screen(render_items);
		
	}




	/*

	██████╗ ███████╗███╗   ██╗██████╗ ███████╗██████╗     ██████╗  █████╗ ████████╗ █████╗
	██╔══██╗██╔════╝████╗  ██║██╔══██╗██╔════╝██╔══██╗    ██╔══██╗██╔══██╗╚══██╔══╝██╔══██╗
	██████╔╝█████╗  ██╔██╗ ██║██║  ██║█████╗  ██████╔╝    ██║  ██║███████║   ██║   ███████║
	██╔══██╗██╔══╝  ██║╚██╗██║██║  ██║██╔══╝  ██╔══██╗    ██║  ██║██╔══██║   ██║   ██╔══██║
	██║  ██║███████╗██║ ╚████║██████╔╝███████╗██║  ██║    ██████╔╝██║  ██║   ██║   ██║  ██║
	╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝╚═════╝ ╚══════╝╚═╝  ╚═╝    ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝ */


	std::vector<Render_item2D> create_loading_screen_render_items() {
		int desiredTotalLines = 40;
		float linesPerPresentHeight = (float)PRESENT_HEIGHT / (float)text_blitter::get_line_height(Bitmap_font_type::STANDARD);
		float scaleRatio = (float)desiredTotalLines / (float)linesPerPresentHeight;
		float loadingScreenWidth = PRESENT_WIDTH * scaleRatio;
		float loadingScreenHeight = PRESENT_HEIGHT * scaleRatio;

		//imblix
		std::string text = "";
		int maxLinesDisplayed = 40;
		int endIndex = asset::get_load_log().size();
		int beginIndex = std::max(0, endIndex - maxLinesDisplayed);
		for (int i = beginIndex; i < endIndex; i++) {
			text += asset::get_load_log()[i] + "\n";
		}

		misk::ivec2 location = misk::ivec2(0.0f, loadingScreenHeight);
		misk::ivec2 viewportSize = misk::ivec2(loadingScreenWidth, loadingScreenHeight);
		//MK_CORE_TRACE("x= {}, y = {}", location.x, location.y);
		return text_blitter::create_text(text, location, viewportSize, Alignment::TOP_LEFT, Bitmap_font_type::STANDARD);

	}
}



