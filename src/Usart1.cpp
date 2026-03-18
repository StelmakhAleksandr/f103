#include "Usart1.h"

#include "core/Gpio.h"
#include "core/Pins.h"

namespace {
std::function<void(char*, int)> printfCallback;
};

extern "C" int _write(int file, char* ptr, int len)
{
    (void)file;
    printfCallback(ptr, len);
    return len;
}

void Usart1::setup(unsigned long long speed, uint32_t baudRate)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    PA9.setMode(GpioMode::Output50Mhz).setOutputMode(GpioOutputMode::AlternateFunctionPushPull).build();
    PA10
        .setMode(GpioMode::Input)
        .setInputMode(GpioInputMode::PullUpDown)
        .setPull(GpioPull::Up)
        .build();

    USART1->BRR = speed / baudRate;
    USART1->CR1 = USART_CR1_TE | USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_RE;
    NVIC_EnableIRQ(USART1_IRQn);

    printfCallback = [this](char* data, int length) {
        for (int i = 0; i < length; ++i) {
            sendByte(static_cast<uint8_t>(data[i]));
        }
    };

    Irq::instance().setUsart1Callback([this]() {
        if (USART1->SR & USART_SR_RXNE) {
            m_rxBuffer.push(static_cast<uint8_t>(USART1->DR & 0xFF));
        }
        if (USART1->SR & USART_SR_TXE) {
            auto byte = m_txBuffer.pop();
            if (byte.has_value()) {
                USART1->DR = byte.value();
            } else {
                USART1->CR1 &= ~USART_CR1_TXEIE;
            }
        }
    });
}

void Usart1::sendByte(uint8_t byte)
{
    IrqGuard guard;
    m_txBuffer.push(byte);
    USART1->CR1 |= USART_CR1_TXEIE;
}

void Usart1::sendString(const char* str)
{
    while (*str) {
        sendByte(static_cast<uint8_t>(*str));
        ++str;
    }
}

std::optional<uint8_t> Usart1::receiveByte()
{
    IrqGuard guard;
    return m_rxBuffer.pop();
}