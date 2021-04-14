add_executable(nandy_tests main.cpp "gate_tests.h")
target_link_libraries(nandy_tests PUBLIC simulation gtest)

add_executable(speed_test speed_test.cpp)
target_link_libraries(speed_test PUBLIC simulation)
