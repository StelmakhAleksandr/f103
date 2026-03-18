#pragma once

#include <array>
#include <cstddef>

template <typename T, std::size_t Size>
class RingBuffer
{
public:
    void push(const T& value)
    {
        if (isFull()) {
            return;
        }
        m_buffer[m_headIndex] = value;
        m_headIndex = (m_headIndex + 1) % Size;
        ++m_count;
    }

    std::optional<T> pop()
    {
        if (isEmpty()) {
            return std::nullopt;
        }
        const T value = m_buffer[m_tailIndex];
        m_tailIndex = (m_tailIndex + 1) % Size;
        --m_count;
        return value;
    }

    bool isEmpty() const
    {
        return m_count == 0;
    }

    bool isFull() const
    {
        return m_count == Size;
    }

    std::size_t size() const
    {
        return m_count;
    }

private:
    std::array<T, Size> m_buffer;
    std::size_t m_headIndex = 0;
    std::size_t m_tailIndex = 0;
    std::size_t m_count = 0;
};