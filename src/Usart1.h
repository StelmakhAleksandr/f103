#pragma once

#include "core/Irq.h"
#include "core/RingBuffer.h"
#include <optional>

class Usart1
{
public:
    void setup(unsigned long long speed, uint32_t baudRate);
    void sendByte(uint8_t byte);
    void sendString(const char* str);
    std::optional<uint8_t> receiveByte();

private:
    RingBuffer<uint8_t, 256> m_txBuffer;
    RingBuffer<uint8_t, 256> m_rxBuffer;
};