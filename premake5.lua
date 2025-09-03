workspace "KEngine"
	architecture "x64"
	
	configurations{
		"Debug",
		"Release",
		"Dist"
	}

	startproject "Sandbox"

outputdir="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project"KEngine"
	location"KEngine"	
	kind"SharedLib"
	language"C++"

	targetdir("bin/"..outputdir.."/%{prj.name}")
	objdir("bin-int/"..outputdir.."/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"

	}

	includedirs{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include"
	}

	filter"system:windows"
	    cppdialect"C++17"
		staticruntime"On"
		systemversion"latest"
		
		defines{
			"KENGINE_PLATFORM_WINDOWS",
			"KENGINE_BUILD_DLL"
		}

		postbuildcommands{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/"..outputdir.."/Sandbox")
		}
		
		buildoptions { "/utf-8" }

	filter "configurations:Debug"
		defines"KENGINE_DEBUG"
		symbols"On"
    filter "configurations:Release"
		defines"KENGINE_RELEASE"	
		optimize"On"
	filter "configurations:Dist"
		defines"KENGINE_DIST"
		optimize"On"

project"Sandbox"
	location"Sandbox"	
	kind"ConsoleApp"
	language"C++"

	targetdir("bin/"..outputdir.."/%{prj.name}")
	objdir("bin-int/"..outputdir.."/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"

	}

	includedirs{
		"KEngine/vendor/spdlog/include",
		"KEngine/src"
	}

	links{
		"KEngine"
	}

	filter"system:windows"
	    cppdialect"C++17"
		staticruntime"On"
		systemversion"latest"
		
		defines{
			"KENGINE_PLATFORM_WINDOWS",
		}
		
		buildoptions { "/utf-8" }

	filter "configurations:Debug"
		defines"KENGINE_DEBUG"
		symbols"On"

    filter "configurations:Release"
		defines"KENGINE_RELEASE"	
		optimize"On"

	filter "configurations:Dist"
		defines"KENGINE_DIST"
		optimize"On"

