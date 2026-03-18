#pragma once

#include "stm32f103x6.h"

enum class GpioMode : uint8_t
{
    Input = 0b00,
    Output10Mhz = 0b01,
    Output2Mhz = 0b10,
    Output50Mhz = 0b11,
};

enum class GpioPull : uint8_t
{
    No,
    Up,
    Down,
};

enum class GpioInputMode : uint8_t
{
    Analog = 0b00,
    Floating = 0b01,
    PullUpDown = 0b10,
};

enum class GpioOutputMode : uint8_t
{
    PushPull = 0b00,
    OpenDrain = 0b01,
    AlternateFunctionPushPull = 0b10,
    AlternateFunctionOpenDrain = 0b11,
};

enum class GpioExtiTrigger : uint8_t
{
    Rising,
    Falling,
    RisingAndFalling,
};

class GpioBuilder;

class GpioPin
{
    friend class GpioBuilder;

public:
    void on();
    void off();
    void toggle();
    bool isHigh() const;
    bool isLow() const;
    GpioPin(GPIO_TypeDef* port, uint8_t pin);

private:
    static void setValue(__IO uint32_t* reg, uint32_t bitsPerValue, uint32_t value, uint8_t bitPosition);
    void setup(GpioMode mode, uint8_t inputOutputMode, GpioPull pull);

    GPIO_TypeDef* m_port;
    uint8_t m_pin;
};

class GpioBuilder
{
public:
    GpioBuilder(GPIO_TypeDef* port, uint8_t pin);
    GpioBuilder& setMode(GpioMode mode);
    GpioBuilder& setPull(GpioPull pull);
    GpioBuilder& setInputMode(GpioInputMode mode);
    GpioBuilder& setOutputMode(GpioOutputMode mode);
    GpioBuilder& enableExti(GpioExtiTrigger trigger, bool enableNvic = true);
    GpioPin build();

private:
    GpioPin m_pin;
    GpioMode m_mode = GpioMode::Input;
    GpioPull m_pull = GpioPull::No;
    uint8_t m_inputOutputMode = (uint8_t)GpioInputMode::Floating;
    bool m_extiEnabled = false;
    bool m_extiEnableNvic = true;
    GpioExtiTrigger m_extiTrigger = GpioExtiTrigger::Falling;
};
