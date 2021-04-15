#include "gtest/gtest.h"

#include "circular_buffer_tests.h"
#include "gate_tests.h"

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
