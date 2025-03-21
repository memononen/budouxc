
local action = _ACTION or ""

workspace "bodouxc"

	location ( "build" )
	configurations { "Debug", "Release" }
	platforms {"native", "x64", "x32"}
  	startproject("example")

	symbols "On"

	filter "configurations:Debug"
		defines { "DEBUG" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	-- windows specific
	filter "system:windows"
		defines { "WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "_HAS_EXCEPTIONS=0" }
		buildoptions { "/W3" }

	filter "platforms:Win32"
		architecture "x32"

	filter "platforms:Win64"
		architecture "x64"

	
	project "example"
		kind "ConsoleApp"
		language "C"
		files { 
			"example/*.c",
			"example/*.h",
			"*.c",
			"*.h",
			"README.md",
		}
		
		includedirs { "example", "" }
		targetdir("build")
	 
		filter "system:windows"
			links { "user32" }

		filter "system:windows"
			buildoptions { "/utf-8" }

		warnings "Extra"
