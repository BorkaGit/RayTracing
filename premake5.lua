-- premake5.lua
workspace "RtApp"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "RtApp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "RtApp"