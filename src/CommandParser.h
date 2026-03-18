#pragma once

#include "Usart1.h"
#include <array>
#include <functional>

class CommandParser
{
public:
    CommandParser(Usart1& usart, std::function<void(const char*)> callback)
        : m_usart(usart)
        , m_callback(callback)
    {
    }

    void poll();

private:
    void processByte(char byte);

    Usart1& m_usart;
    std::function<void(const char*)> m_callback;
    std::array<char, 256> m_commandBuffer {};
    std::size_t m_commandLength = 0;
};