workspace "radu_chira_virtual_machine"
	configurations { "Debug", "Release" }
	architecture "x32"
	flags {"RelativeLinks", "UndefinedIdentifiers", "ShadowedVariables"}

	project "rcvm"
		kind "SharedLib"
		language "C"
		location "rcvm"
		
		targetdir "bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
		objdir "bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
	
		files { "%{prj.name}/src/**.c", "%{prj.name}/src/**.h" }
		
		defines { "DLL_EXPORT" }

		filter "configurations:Debug"
			symbols "On"
			defines { "DEBUG" }
		filter "configurations:Release"
			optimize "On"
			defines { "RELEASE" }

	project "sandbox"
		kind "ConsoleApp"
		language "C"
		location "sandbox"

		targetdir "bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
		objdir "bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

		files { "%{prj.name}/src/**.c", "%{prj.name}/src/**.h" }

		includedirs { "rcvm/src" }
		
		links { "rcvm" }
		
		defines { "DLL_IMPORT" }
		filter "configurations:Debug"
			symbols "On"
			defines { "DEBUG" }
		filter "configurations:Release"
			optimize "On"
			defines { "RELEASE" }
