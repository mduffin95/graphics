cmake_minimum_required(VERSION 3.6)
project(Computer_Graphics)

set(CMAKE_CXX_STANDARD 11)

INCLUDE_DIRECTORIES(/usr/local/Cellar/sdl/1.2.15/include/SDL)
INCLUDE_DIRECTORIES($(S_DIR)/SDLauxiliary.h)
INCLUDE_DIRECTORIES($(S_DIR)/TestModel.h)



set(SOURCE_FILES
    Source/SDLauxiliary.h
        Source/Rasteriser.cpp
    Source/TestModel.h
    Makefile Source/Tools.h Source/Light.h Source/Renderer.h Source/Rasteriser.h Source/main.cpp)

add_executable(Computer_Graphics ${SOURCE_FILES})