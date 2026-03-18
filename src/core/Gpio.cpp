#include "Gpio.h"

namespace {
IRQn_Type extiIrqFromPin(uint8_t pin)
{
    if (pin <= 4U) {
        return static_cast<IRQn_Type>(EXTI0_IRQn + pin);
    }
    if (pin <= 9U) {
        return EXTI9_5_IRQn;
    }
    return EXTI15_10_IRQn;
}
}

GpioPin::GpioPin(GPIO_TypeDef* port, uint8_t pin)
    : m_port(port)
    , m_pin(pin)
{
}

void GpioPin::on()
{
    m_port->BSRR = 1UL << m_pin;
}

void GpioPin::off()
{
    m_port->BSRR = 1UL << (m_pin + 16);
}

void GpioPin::toggle()
{
    uint32_t mask = 1UL << m_pin;
    if (m_port->ODR & mask)
        m_port->BSRR = mask << 16;
    else
        m_port->BSRR = mask;
}

bool GpioPin::isHigh() const
{
    uint32_t mask = 1UL << m_pin;
    return (m_port->IDR & mask) != 0U;
}

bool GpioPin::isLow() const
{
    return !isHigh();
}

void GpioPin::setup(GpioMode mode, uint8_t inputOutputMode, GpioPull pull)
{
    auto portIndex = ((uint32_t)m_port - GPIOA_BASE) / (GPIOB_BASE - GPIOA_BASE);
    RCC->APB2ENR |= (0x1UL << (RCC_APB2ENR_IOPAEN_Pos + portIndex));

    __IO uint32_t* reg = (m_pin >= 8) ? &m_port->CRH : &m_port->CRL;
    uint8_t pinIndex = m_pin % 8;
    setValue(reg, 2, (uint32_t)mode, pinIndex * 4);
    setValue(reg, 2, (uint32_t)inputOutputMode, pinIndex * 4 + 2);

    if ((mode == GpioMode::Input) && (inputOutputMode == (uint8_t)GpioInputMode::PullUpDown)) {
        uint32_t mask = 1UL << m_pin;

        if (pull == GpioPull::Up) {
            m_port->BSRR = mask;
        } else if (pull == GpioPull::Down) {
            m_port->BSRR = mask << 16;
        } else {
        }
    }
}

void GpioPin::setValue(__IO uint32_t* reg, uint32_t bitsPerValue, uint32_t value, uint8_t bitPosition)
{
    const uint32_t mask = (1UL << bitsPerValue) - 1UL;
    *reg = (*reg & ~(mask << bitPosition)) | ((value & mask) << bitPosition);
}

GpioBuilder::GpioBuilder(GPIO_TypeDef* port, uint8_t pin)
    : m_pin(port, pin)
{
}

GpioBuilder& GpioBuilder::setMode(GpioMode mode)
{
    m_mode = mode;
    return *this;
}

GpioBuilder& GpioBuilder::setPull(GpioPull pull)
{
    m_pull = pull;
    return *this;
}

GpioBuilder& GpioBuilder::setInputMode(GpioInputMode mode)
{
    m_inputOutputMode = (uint8_t)mode;
    return *this;
}

GpioBuilder& GpioBuilder::setOutputMode(GpioOutputMode mode)
{
    m_inputOutputMode = (uint8_t)mode;
    return *this;
}

GpioBuilder& GpioBuilder::enableExti(GpioExtiTrigger trigger, bool enableNvic)
{
    m_extiEnabled = true;
    m_extiTrigger = trigger;
    m_extiEnableNvic = enableNvic;
    return *this;
}

GpioPin GpioBuilder::build()
{
    m_pin.setup(m_mode, m_inputOutputMode, m_pull);

    if (m_extiEnabled) {
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

        uint32_t portIndex = ((uint32_t)m_pin.m_port - GPIOA_BASE) / (GPIOB_BASE - GPIOA_BASE);
        uint8_t exticrIndex = m_pin.m_pin / 4U;
        uint8_t exticrOffset = (m_pin.m_pin % 4U) * 4U;

        AFIO->EXTICR[exticrIndex] &= ~(0xFUL << exticrOffset);
        AFIO->EXTICR[exticrIndex] |= (portIndex << exticrOffset);

        uint32_t mask = 1UL << m_pin.m_pin;
        EXTI->IMR |= mask;

        if (m_extiTrigger == GpioExtiTrigger::Rising || m_extiTrigger == GpioExtiTrigger::RisingAndFalling) {
            EXTI->RTSR |= mask;
        } else {
            EXTI->RTSR &= ~mask;
        }

        if (m_extiTrigger == GpioExtiTrigger::Falling || m_extiTrigger == GpioExtiTrigger::RisingAndFalling) {
            EXTI->FTSR |= mask;
        } else {
            EXTI->FTSR &= ~mask;
        }

        EXTI->PR = mask;

        if (m_extiEnableNvic) {
            NVIC_EnableIRQ(extiIrqFromPin(m_pin.m_pin));
        }
    }

    return m_pin;
}