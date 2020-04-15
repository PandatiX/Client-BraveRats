cmake_minimum_required(VERSION 3.13)
project(ClientBraveRats)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp lib/glad.c ../lib/imgui/imgui.cpp ../lib/imgui/imgui_draw.cpp
        ../lib/imgui/imgui_widgets.cpp ../lib/imgui/imgui_impl_glfw.cpp ../lib/imgui/imgui_impl_opengl3.cpp
        src/Window.cpp src/Application.cpp src/exceptions/Exception.cpp src/Terminal.cpp src/Game.cpp)

include_directories( SYSTEM "lib/glad/include")
add_executable(ClientBraveRats ${SOURCE_FILES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lglfw -lGL -ldl -std=c++11")
target_link_libraries(ClientBraveRats ${CMAKE_DL_LIBS} glfw ${GLFW_LIBRARIES} ${X11_LIBRARIES})