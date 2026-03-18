#pragma once

#include <functional>
#include <stdint.h>

class Adc1
{
public:
    void setup(std::function<void(int value)> callback);
    void startConversion();
    uint16_t value() const;

private:
    uint16_t m_lastValue = 0;
    std::function<void(int value)> m_callback;
};