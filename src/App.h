#pragma once

#include "core/Clocks.h"
#include "core/Irq.h"
#include "core/Pins.h"
#include "core/Timer.h"
#include <optional>

#include "Adc1.h"
#include "CommandParser.h"
#include "Usart1.h"

class App
{
public:
    void setup();
    void loop();

private:
    std::optional<GpioPin> m_ledPA12;
    std::optional<GpioPin> m_buttonPb5;
    std::optional<Clocks> m_clocks;
    std::optional<Timer> m_pwmTimer;
    std::optional<Timer> m_baseTimer;
    Usart1 m_usart1;
    std::optional<CommandParser> m_commandParser;
    Adc1 m_adc1;
    int m_adcPercentage = 0;
};