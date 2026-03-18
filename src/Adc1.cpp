#include "Adc1.h"

#include "core/Irq.h"
#include "core/Pins.h"
#include <chrono>
#include <thread>

void Adc1::setup(std::function<void(int value)> callback)
{
    using namespace std::chrono_literals;

    m_callback = callback;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    PA0.setMode(GpioMode::Input).setInputMode(GpioInputMode::Analog).build();

    ADC1->SMPR2 &= ~ADC_SMPR2_SMP0;
    ADC1->SMPR2 |= (0b111 << ADC_SMPR2_SMP0_Pos); // 239.5 cycles

    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |= (0UL << ADC_SQR1_L_Pos);
    ADC1->SQR3 = 0; // channel 0

    ADC1->CR1 = ADC_CR1_EOCIE;

    ADC1->CR2 = 0UL;

    ADC1->CR2 |= ADC_CR2_ADON;
    std::this_thread::sleep_for(10ms);

    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while ((ADC1->CR2 & ADC_CR2_RSTCAL) != 0) {
    }

    ADC1->CR2 |= ADC_CR2_CAL;
    while ((ADC1->CR2 & ADC_CR2_CAL) != 0) {
    }

    ADC1->CR2 |= (0b111 << ADC_CR2_EXTSEL_Pos);
    ADC1->CR2 |= ADC_CR2_EXTTRIG;

    Irq::instance().setAdc1_2Callback([this]() {
        if (ADC1->SR & ADC_SR_EOC) {
            m_lastValue = static_cast<uint16_t>(ADC1->DR & 0xFFFF);
            if (m_callback) {
                m_callback(m_lastValue);
            }
        }
    });
    NVIC_EnableIRQ(ADC1_2_IRQn);
}

void Adc1::startConversion()
{
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

uint16_t Adc1::value() const
{
    return m_lastValue;
}