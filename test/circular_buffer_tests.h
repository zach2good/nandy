#pragma once

#include "definitions.h"
#include "simulation.h"

TEST(circular_buffer_tests, construction)
{ 
    circular_buffer_t<std::size_t> q;

    EXPECT_EQ(q.size(), 0);
    EXPECT_EQ(q.empty(), true);
}

TEST(circular_buffer_tests, construction_default_1024)
{
    circular_buffer_t<std::size_t> q;

    EXPECT_EQ(q.size(), 0);
    EXPECT_EQ(q.empty(), true);
}

TEST(circular_buffer_tests, push)
{
    circular_buffer_t<std::size_t> q;
    std::size_t input = 1;

    q.push(input);

    EXPECT_EQ(q.size(), 1);
    EXPECT_EQ(q.empty(), false);
    EXPECT_EQ(q.front(), input);
}

TEST(circular_buffer_tests, push_pop)
{
    circular_buffer_t<std::size_t> q;
    std::size_t input = 1;

    q.push(input);

    EXPECT_EQ(q.front(), input);

    q.pop();

    EXPECT_EQ(q.size(), 0);
    EXPECT_EQ(q.empty(), true);
}

TEST(circular_buffer_tests, push_overwrite)
{
    circular_buffer_t<std::size_t> q;

    for (int i = 0; i <= q.capacity(); ++i)
    {
        q.push(i);
    }

    EXPECT_EQ(q.empty(), false);

    q.push(9999);

    EXPECT_EQ(q.empty(), false);
}
