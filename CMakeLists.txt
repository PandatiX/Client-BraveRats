cmake_minimum_required(VERSION 3.15)
project(ClientBraveRats)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp lib/glad.c ../lib/imgui/imgui.cpp ../lib/imgui/imgui_draw.cpp
        ../lib/imgui/imgui_widgets.cpp ../lib/imgui/imgui_impl_glfw.cpp ../lib/imgui/imgui_impl_opengl3.cpp
        src/Window.cpp src/Application.cpp src/exceptions/Exception.cpp src/Terminal.cpp src/barriers/BarrierGMLS.hpp)

add_executable(ClientBraveRats ${SOURCE_FILES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lglfw -lGL -ldl -std=c++11")