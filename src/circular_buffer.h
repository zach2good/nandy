#pragma once

#include <array>
#include <cstdint>
#include <vector>

template <typename T>
class circular_buffer_t
{
private:
    std::size_t m_head;
    std::size_t m_tail;
    std::size_t m_size;
    std::array<T, 1024> m_data;

public:
    circular_buffer_t()
    : m_head(0)
    , m_tail(0)
    , m_size(0)
    , m_data({})
    {}

    void push(T val)
    {
        m_data[m_tail] = val;
        if (++m_tail >= m_data.size())
        {
            m_tail -= m_data.size();
        }

        if (m_head == m_tail)
        {
            if (++m_head >= m_data.size())
            {
                m_head -= m_data.size();
            }
        }
        else
        {
            ++m_size;
        }
    }

    void pop()
    {
        if (++m_head >= m_data.size())
        {
            m_head -= m_data.size();
        }
        --m_size;
    }

    bool empty()
    {
        return size() == 0;
    }

    std::size_t size()
    {
        return m_size;
    }

    std::size_t capacity()
    {
        return m_data.size();
    }

    T front()
    {
        return m_data[m_head];
    }
};
