add_executable(nandy_tests main.cpp "gate_tests.h")
target_link_libraries(nandy_tests PUBLIC simulation gtest)
