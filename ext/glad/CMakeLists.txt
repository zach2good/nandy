set_source_files_properties(src/glad.c PROPERTIES LANGUAGE CXX)
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/glad ${CMAKE_CURRENT_SOURCE_DIR}/include/KHR)
