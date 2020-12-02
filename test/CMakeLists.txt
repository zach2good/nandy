add_executable(nandy_tests main.cpp "basic_tests.h" "compound_component_tests.h")
target_link_libraries(nandy_tests PUBLIC simulation gtest)
