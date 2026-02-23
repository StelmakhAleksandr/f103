#include "Led1.h"

void Led1::setup()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
    GPIOA->CRH |= GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1;
}

void Led1::on()
{
    GPIOA->BSRR = GPIO_BSRR_BS12;
}

void Led1::off()
{
    GPIOA->BSRR = GPIO_BSRR_BR12;
}

void Led1::toggle()
{
    if ((GPIOA->ODR & GPIO_ODR_ODR12) == 0) {
        on();
    } else {
        off();
    }
}