workspace "Misk2025_V1"
	
	architecture "x64"
	startproject "Misk2025_V1"


	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

IncludeDir = {}
IncludeDir["GLFW"] = "Misk2025_V1/vendor/GLFW/include"
IncludeDir["Glad"] = "Misk2025_V1/vendor/Glad/include"
--IncludeDir["imgui"] = "Misk/vendor/imgui"
IncludeDir["glm"] = "Misk2025_V1/vendor/glm"
IncludeDir["stb"] = "Misk2025_V1/vendor/STB/include"
IncludeDir["tinyexr"] = "Misk2025_V1/vendor/tinyexr"
IncludeDir["assimp"] = "Misk2025_V1/vendor/assimp/include"

include "Misk2025_V1/vendor/Glad"
include "Misk2025_V1/vendor/GLFW"
--include "Misk2025_V1/vendor/imgui"

project "Misk2025_V1"
	
	location "Misk2025_V1"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")



	pchheader "mkpch.h"
	pchsource "Misk2025_V1/engine/mkpch.cpp"

	files
	{
		"%{prj.name}/engine/**.h",
		"%{prj.name}/engine/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/engine",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		--"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.tinyexr}",
		"%{IncludeDir.assimp}"
	}

	libdirs
	{
		"Misk2025_V1/vendor/GLFW/lib-vc2022",
		"Misk2025_V1/vendor/assimp/lib"
	}

	links {
		"GLFW",
		"Glad",
		"opengl32",
		"assimp-vc143-mt.lib"
	}

	postbuildcommands {
		'{COPY} "%{wks.location}/Misk2025_V1/vendor/dll/assimp-vc143-mt.dll" "%{cfg.targetdir}"'
	}



	filter "system:windows"
		systemversion "latest"


		buildoptions { "/utf-8" }

		defines
		{
			"MK_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
			"MK_ENABLE_ASSERTS",
			
		}


	filter "configurations:Debug"
		defines "MK_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "MK_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "MK_DIST"
		runtime "Release"
		optimize "on"





