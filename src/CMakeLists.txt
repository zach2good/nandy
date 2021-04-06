add_library(simulation STATIC simulation.cpp)
target_include_directories(simulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(nandy main.cpp window.cpp)
target_link_libraries(nandy PUBLIC simulation glad SDL2-static SDL2main imgui)
