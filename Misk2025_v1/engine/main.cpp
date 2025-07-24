#include <glm/glm.hpp>
#include "mkpch.h"
#include <core/misk_type.h>
#include <core/application.h>





extern "C" {
	__declspec(dllexport) unsigned __int32 AmdPowerXpressRequestHighPerformance = 0x1;
	__declspec(dllexport) unsigned __int32 NvOptimusEnablement = 0x1;
}


int main()
{

	Application_config app_config;
	app_config.start_window_height = 1080 * 0.75;
	app_config.start_window_width = 1920 * 0.75;
	app_config.title = "Imblix window";

	application_create(&app_config);
	application_run();


	return 0;
}









