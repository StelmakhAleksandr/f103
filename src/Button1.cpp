#include "Button1.h"

Button1 buttonPB5;

void Button1::setup()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    GPIOB->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
    GPIOB->CRL |= GPIO_CRL_CNF5_1;

    GPIOB->BSRR = GPIO_BSRR_BR5;

    AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI5_PB;

    EXTI->IMR |= EXTI_IMR_MR5;
    EXTI->FTSR |= EXTI_FTSR_TR5;
    EXTI->RTSR |= EXTI_RTSR_TR5;
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    EXTI->PR = EXTI_PR_PR5;
}

bool Button1::pressed() const
{
    return (GPIOB->IDR & GPIO_IDR_IDR5) != 0;
}

void Button1::setCallback(std::function<void(bool)> callback)
{
    this->callback = callback;
}

void Button1::handleInterrupt()
{
    // if (pressed()) {
    //     GPIOA->BSRR = GPIO_BSRR_BS12;
    // } else {
    //     GPIOA->BSRR = GPIO_BSRR_BR12;
    // }
    if (callback) {
        callback(pressed());
    }
}