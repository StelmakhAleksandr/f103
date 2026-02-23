#include "Irq.h"

#include "Button1.h"

extern "C" void SysTick_Handler()
{
    msTicks++;
}

void delay(uint32_t ms)
{
    uint32_t targetTicks = msTicks + ms;
    while (msTicks <= targetTicks) {
        __WFI();
    }
}

extern "C" void EXTI2_IRQHandler()
{
    if (EXTI->PR & EXTI_PR_PR2) {
        EXTI->PR = EXTI_PR_PR2;

        if ((GPIOA->IDR & GPIO_IDR_IDR2) == 0) {
            GPIOA->BSRR = GPIO_BSRR_BS12;
        } else {
            GPIOA->BSRR = GPIO_BSRR_BR12;
        }
    }
}

extern "C" void EXTI9_5_IRQHandler()
{
    if (EXTI->PR & EXTI_PR_PR5) {
        EXTI->PR = EXTI_PR_PR5;
        buttonPB5.handleInterrupt();
    }
}

extern "C" void sleep(uint32_t seconds)
{
    uint32_t targetTicks = msTicks + seconds * 1000U;
    while (msTicks <= targetTicks) {
        __WFI();
    }
}

extern "C" void usleep(uint32_t microseconds)
{
    uint32_t targetTicks = msTicks + microseconds / 1000U;
    while (msTicks <= targetTicks) {
        __WFI();
    }
}

extern "C" void TIM1_UP_IRQHandler()
{
    if (TIM1->SR & TIM_SR_UIF) {
        TIM1->SR = ~TIM_SR_UIF;
        GPIOA->ODR ^= GPIO_ODR_ODR13;
    }
}

extern "C" void TIM2_IRQHandler()
{
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR = ~TIM_SR_UIF;
        GPIOB->ODR ^= GPIO_ODR_ODR3;
    }
}

extern "C" void TIM3_IRQHandler()
{
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR = ~TIM_SR_UIF;
        GPIOB->ODR ^= GPIO_ODR_ODR7;
    }
}