add_subdirectory(glad)
add_subdirectory(SDL2)

# Define imgui build here
add_library(imgui
	imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_widgets.cpp
	imgui/backends/imgui_impl_opengl3.cpp
	imgui/backends/imgui_impl_sdl.cpp)

target_include_directories(imgui PUBLIC imgui imgui/backends)
target_link_libraries(imgui PUBLIC glad SDL2-static)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest)

add_library(json INTERFACE)
target_include_directories(json INTERFACE json)
