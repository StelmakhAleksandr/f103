#pragma once

#include "Pins.h"
#include "stm32f103x6.h"

#include <array>
#include <cstdint>

class TimerBuilder;

class Timer
{
    friend class TimerBuilder;

public:
    enum class Channel : uint8_t
    {
        Ch1 = 0,
        Ch2 = 1,
        Ch3 = 2,
        Ch4 = 3,
    };

    enum class PwmMode : uint8_t
    {
        Mode1,
        Mode2,
    };

    enum class PwmPinout : uint8_t
    {
        Auto,
        Tim1NoRemap,
        Tim1PartialRemap,
        Tim2NoRemap,
        Tim2PartialRemap1,
        Tim2PartialRemap2,
        Tim2FullRemap,
        Tim3NoRemap,
        Tim3PartialRemap,
        Tim3FullRemap,
    };

    struct PwmChannelOptions
    {
        PwmChannelOptions(PwmMode mode = PwmMode::Mode1,
            bool mainInverted = false,
            bool enableComplementary = false,
            bool complementaryInverted = false,
            PwmPinout pinout = PwmPinout::Auto)
            : mode(mode)
            , mainInverted(mainInverted)
            , enableComplementary(enableComplementary)
            , complementaryInverted(complementaryInverted)
            , pinout(pinout)
        {
        }

        PwmMode mode;
        bool mainInverted;
        bool enableComplementary;
        bool complementaryInverted;
        PwmPinout pinout;
    };

    Timer() = default;

    void start();
    void stop();

    void setPrescaler(uint16_t prescaler);
    void setPeriod(uint32_t period);
    uint32_t getPeriod() const;

    void setCompare(Channel channel, uint32_t compare);

    static TimerBuilder builder(TIM_TypeDef* instance);

private:
    struct PwmChannelConfig
    {
        bool enabled = false;
        Channel channel = Channel::Ch1;
        PwmMode mode = PwmMode::Mode1;
        bool mainInverted = false;
        bool enableComplementary = false;
        bool complementaryInverted = false;
        PwmPinout pinout = PwmPinout::Auto;
        uint32_t compare = 0U;
    };

    explicit Timer(TIM_TypeDef* instance);

    static void enablePeripheralClock(TIM_TypeDef* instance);
    static bool isAdvancedTimer(TIM_TypeDef* instance);

    void configurePwmPins(const PwmChannelConfig& config);
    void applyPwmChannelConfig(const PwmChannelConfig& config);
    void enableMainOutputIfNeeded(bool pwmConfigured);

    TIM_TypeDef* m_instance = nullptr;
    uint32_t m_period = 0U;
};

class TimerBuilder
{
public:
    explicit TimerBuilder(TIM_TypeDef* instance);

    TimerBuilder& setPrescaler(uint16_t prescaler);
    TimerBuilder& setPeriod(uint32_t period);
    TimerBuilder& enableUpdateInterrupt(bool enabled = true);
    TimerBuilder& enableAutoReloadPreload(bool enabled = true);
    TimerBuilder& configurePwmChannel(Timer::Channel channel, uint32_t compare);
    TimerBuilder& configurePwmChannel(Timer::Channel channel,
        uint32_t compare,
        Timer::PwmChannelOptions options);

    Timer build();

private:
    TIM_TypeDef* m_instance = nullptr;
    uint16_t m_prescaler = 0U;
    uint32_t m_period = 999U;
    bool m_enableUpdateInterrupt = false;
    bool m_enableAutoReloadPreload = true;
    std::array<Timer::PwmChannelConfig, 4> m_pwmChannels {};
};
