#include "Irq.h"

std::unique_ptr<Irq> Irq::s_instance = nullptr;
Irq& Irq::instance()
{
    if (!s_instance) {
        s_instance = std::make_unique<Irq>(PrivateConstructorTag {});
    }
    return *s_instance;
}

void Irq::setExti5Callback(std::function<void()> callback)
{
    m_exti5Callback = callback;
}

void Irq::setTim1Callback(std::function<void()> callback)
{
    m_Tim1Callback = callback;
}

void Irq::setTim2Callback(std::function<void()> callback)
{
    m_Tim2Callback = callback;
}

void Irq::setTim3Callback(std::function<void()> callback)
{
    m_Tim3Callback = callback;
}

void Irq::setUsart1Callback(std::function<void()> callback)
{
    m_usart1Callback = callback;
}

void Irq::setAdc1_2Callback(std::function<void()> callback)
{
    m_adc1_2Callback = callback;
}

extern "C" void EXTI9_5_IRQHandler()
{
    if (EXTI->PR & EXTI_PR_PR5) {
        EXTI->PR = EXTI_PR_PR5;
        if (Irq::instance().m_exti5Callback) {
            Irq::instance().m_exti5Callback();
        }
    }
}

extern "C" void TIM1_UP_IRQHandler()
{
    if (TIM1->SR & TIM_SR_UIF) {
        TIM1->SR = ~TIM_SR_UIF;
        if (Irq::instance().m_Tim1Callback) {
            Irq::instance().m_Tim1Callback();
        }
    }
}

extern "C" void TIM2_IRQHandler()
{
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR = ~TIM_SR_UIF;
        if (Irq::instance().m_Tim2Callback) {
            Irq::instance().m_Tim2Callback();
        }
    }
}

extern "C" void TIM3_IRQHandler()
{
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR = ~TIM_SR_UIF;
        if (Irq::instance().m_Tim3Callback) {
            Irq::instance().m_Tim3Callback();
        }
    }
}

extern "C" void USART1_IRQHandler()
{
    if (Irq::instance().m_usart1Callback) {
        Irq::instance().m_usart1Callback();
    }
}

extern "C" void ADC1_2_IRQHandler()
{
    if (Irq::instance().m_adc1_2Callback) {
        Irq::instance().m_adc1_2Callback();
    }
}