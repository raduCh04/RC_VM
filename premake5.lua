workspace "8086"
	configurations { "Debug", "Release" }
	architecture "x32"

	project "8086"
		kind "ConsoleApp"
		language "C"
		location "8086"
		
		targetdir "bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
		objdir "bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
	
		files { "**.c", "**.h" }

		filter "configurations:Debug"
			symbols "On"
			defines { "DEBUG" }
		filter "configurations:Release"
			optimize "On"
			defines { "RELEASE" }
