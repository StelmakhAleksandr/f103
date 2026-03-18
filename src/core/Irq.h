#pragma once

#include "stm32f103x6.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>

extern "C" void SysTick_Handler();
extern "C" void EXTI9_5_IRQHandler();
extern "C" void TIM1_UP_IRQHandler();
extern "C" void TIM2_IRQHandler();
extern "C" void TIM3_IRQHandler();
extern "C" void USART1_IRQHandler();
extern "C" void ADC1_2_IRQHandler();

class Irq
{
    friend void SysTick_Handler();
    friend void EXTI9_5_IRQHandler();
    friend void TIM1_UP_IRQHandler();
    friend void TIM2_IRQHandler();
    friend void TIM3_IRQHandler();
    friend void USART1_IRQHandler();
    friend void ADC1_2_IRQHandler();
    struct PrivateConstructorTag
    {
    };

public:
    static Irq& instance();
    Irq(PrivateConstructorTag tag) {};
    void setExti5Callback(std::function<void()> callback);
    void setTim1Callback(std::function<void()> callback);
    void setTim2Callback(std::function<void()> callback);
    void setTim3Callback(std::function<void()> callback);
    void setUsart1Callback(std::function<void()> callback);
    void setAdc1_2Callback(std::function<void()> callback);

private:
    static std::unique_ptr<Irq> s_instance;

    std::function<void()> m_exti5Callback;
    std::function<void()> m_Tim1Callback;
    std::function<void()> m_Tim2Callback;
    std::function<void()> m_Tim3Callback;
    std::function<void()> m_usart1Callback;
    std::function<void()> m_adc1_2Callback;
};

class IrqGuard
{
public:
    IrqGuard()
    {
        m_state = __get_PRIMASK();
        __disable_irq();
    }

    ~IrqGuard()
    {
        if ((m_state & 1U) == 0U) {
            __enable_irq();
        }
    }

private:
    uint32_t m_state;
};
