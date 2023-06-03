workspace "radu_chira_virtual_machine"
	configurations { "Debug", "Release" }
	architecture "x32"
	flags {"RelativeLinks", "UndefinedIdentifiers", "ShadowedVariables"}

	project "rcvm"
		kind "ConsoleApp"
		language "C"
		location "rcvm"
		
		targetdir "%{prj.location}"--"bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
		objdir "bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
	
		files { "%{prj.name}/**.c", "%{prj.name}/**.h" }

		filter "configurations:Debug"
			symbols "On"
			defines { "DEBUG" }
		filter "configurations:Release"
			optimize "On"
			defines { "RELEASE" }