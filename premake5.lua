-- premake5.lua

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

workspace "GA_String_Drawing"
   configurations { "Debug", "Release" }
   platforms { "x64", "x86"}
   architecture "x86_64"
   startproject "GA_String_Drawing"
   cppdialect "C++20"

include("vendor/raylib_premake5.lua")
project "GA_String_Drawing"
   kind "ConsoleApp"
   language "C++"
   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
   location("%{wks.location}/%{prj.name}")

   includedirs{
      "%{wks.location}/vendor/Genetic_Algorithm_Cpp/Genetic_Algorithm_Cpp/include"
   }
   
   include_raylib()
   files { "%{prj.name}/**.h","Examples/%{prj.name}/**.hpp","%{prj.name}/**.cpp" }


   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      
      filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
   link_raylib()




   

