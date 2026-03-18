#include "Clocks.h"

#include "stm32f103x6.h"

namespace {
constexpr uint32_t HsiFreq = 8_MHz;
constexpr int minPllMul = 2;
constexpr int maxPllMul = 16;
constexpr uint32_t SysTickLoadMask = 0x00FFFFFFU;

void configureSysTick(uint32_t hclkHz, uint32_t tickRateHz)
{
    if (tickRateHz == 0U) {
        return;
    }

    const uint32_t reload = (hclkHz / tickRateHz) - 1U;
    if (reload > SysTickLoadMask) {
        return;
    }

    SysTick->VAL = 0U;
    SysTick->LOAD = reload;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
}
}

unsigned long long Clocks::pclk1Timers() const
{
    return m_pclk1 * 2;
}

unsigned long long Clocks::pclk1() const
{
    return m_pclk1;
}

unsigned long long Clocks::pclk2() const
{
    return m_pclk2;
}

unsigned long long Clocks::hclk() const
{
    return m_hclk;
}

unsigned long long Clocks::sysclk() const
{
    return m_sysclk;
}

void Clocks::runHse()
{
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0) {
    }
}

void Clocks::runHsi()
{
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {
    }
}

void Clocks::runPll()
{
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
    }
}

void Clocks::runPllclk()
{
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
    }
}

void Clocks::setupPll(uint32_t mul)
{
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= mul << RCC_CFGR_PLLMULL_Pos;
    if (m_hse > 0_MHz) {
        RCC->CFGR |= RCC_CFGR_PLLSRC;
    }
}

void Clocks::setupBusPrescalers(AhbPrescaler ahb, Apb1Prescaler apb1, Apb2Prescaler apb2)
{
    uint32_t ahbMask = (ahb == AhbPrescaler::Div1) ? RCC_CFGR_HPRE_DIV1
        : (ahb == AhbPrescaler::Div2)              ? RCC_CFGR_HPRE_DIV2
        : (ahb == AhbPrescaler::Div4)              ? RCC_CFGR_HPRE_DIV4
        : (ahb == AhbPrescaler::Div8)              ? RCC_CFGR_HPRE_DIV8
        : (ahb == AhbPrescaler::Div16)             ? RCC_CFGR_HPRE_DIV16
        : (ahb == AhbPrescaler::Div64)             ? RCC_CFGR_HPRE_DIV64
        : (ahb == AhbPrescaler::Div128)            ? RCC_CFGR_HPRE_DIV128
        : (ahb == AhbPrescaler::Div256)            ? RCC_CFGR_HPRE_DIV256
                                                   : RCC_CFGR_HPRE_DIV512;

    uint32_t apb1Mask = (apb1 == Apb1Prescaler::Div1) ? RCC_CFGR_PPRE1_DIV1
        : (apb1 == Apb1Prescaler::Div2)               ? RCC_CFGR_PPRE1_DIV2
        : (apb1 == Apb1Prescaler::Div4)               ? RCC_CFGR_PPRE1_DIV4
        : (apb1 == Apb1Prescaler::Div8)               ? RCC_CFGR_PPRE1_DIV8
                                                      : RCC_CFGR_PPRE1_DIV16;

    uint32_t apb2Mask = (apb2 == Apb2Prescaler::Div1) ? RCC_CFGR_PPRE2_DIV1
        : (apb2 == Apb2Prescaler::Div2)               ? RCC_CFGR_PPRE2_DIV2
        : (apb2 == Apb2Prescaler::Div4)               ? RCC_CFGR_PPRE2_DIV4
        : (apb2 == Apb2Prescaler::Div8)               ? RCC_CFGR_PPRE2_DIV8
                                                      : RCC_CFGR_PPRE2_DIV16;

    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2)) | ahbMask | apb1Mask | apb2Mask;
    m_hclk = m_sysclk / static_cast<uint32_t>(ahb);
    m_pclk1 = m_hclk / static_cast<uint32_t>(apb1);
    m_pclk2 = m_hclk / static_cast<uint32_t>(apb2);
}

void Clocks::setupAdcPrescaler(AdcPrescaler adc)
{
    uint32_t adcMask = (adc == AdcPrescaler::Div2) ? RCC_CFGR_ADCPRE_DIV2 : (adc == AdcPrescaler::Div4) ? RCC_CFGR_ADCPRE_DIV4
        : (adc == AdcPrescaler::Div6)                                                                   ? RCC_CFGR_ADCPRE_DIV6
                                                                                                        : RCC_CFGR_ADCPRE_DIV8;

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_ADCPRE) | adcMask;
}

void Clocks::setFlashLatency()
{
    FLASH->ACR &= ~FLASH_ACR_LATENCY;

    uint32_t latencyBits = 0;

    if (m_sysclk <= 24_MHz) {
        latencyBits = 0b000;
    } else if (m_sysclk <= 48_MHz) {
        latencyBits = 0b001;
    } else {
        latencyBits = 0b010;
    }

    FLASH->ACR |= (latencyBits << FLASH_ACR_LATENCY_Pos);
}

void Clocks::enableFlashPrefetchAndHalfCycle()
{
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR &= ~FLASH_ACR_HLFCYA;
}

void Clocks::switchSysclkToHsi()
{
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {
    }

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
    }
}

void Clocks::disablePll()
{
    RCC->CR &= ~RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) != 0) {
    }
}

ClocksBuilder& ClocksBuilder::setSysclk(unsigned long long value)
{
    m_clock.m_sysclk = value;
    return *this;
}

ClocksBuilder& ClocksBuilder::setHse(unsigned long long value)
{
    m_clock.m_hse = value;
    return *this;
}

ClocksBuilder& ClocksBuilder::setPrescalers(AhbPrescaler ahb, Apb1Prescaler apb1, Apb2Prescaler apb2)
{
    m_ahb = ahb;
    m_apb1 = apb1;
    m_apb2 = apb2;
    return *this;
}

ClocksBuilder& ClocksBuilder::setAdcPrescaler(AdcPrescaler adc)
{
    m_adc = adc;
    return *this;
}

ClocksBuilder& ClocksBuilder::setSysTick(uint32_t tickRateHz)
{
    m_configureSysTick = true;
    m_sysTickHz = tickRateHz;
    return *this;
}

Clocks ClocksBuilder::build()
{
    m_clock.switchSysclkToHsi();
    m_clock.disablePll();

    if (m_clock.m_hse > 0_MHz) {
        m_clock.runHse();
    } else {
        m_clock.runHsi();
    }
    m_clock.setupPll(calculatePllMul());
    m_clock.runPll();
    m_clock.setupBusPrescalers(m_ahb, m_apb1, m_apb2);
    m_clock.setupAdcPrescaler(m_adc);
    m_clock.setFlashLatency();
    m_clock.enableFlashPrefetchAndHalfCycle();
    m_clock.runPllclk();

    if (m_configureSysTick) {
        configureSysTick(static_cast<uint32_t>(m_clock.hclk()), m_sysTickHz);
    }

    return m_clock;
}

uint32_t ClocksBuilder::calculatePllMul()
{
    auto baseClock = m_clock.m_hse > 0_MHz ? m_clock.m_hse : (HsiFreq / 2);
    for (auto factor = minPllMul; factor < maxPllMul + 1; factor++) {
        auto pllOut = baseClock * factor;
        if (pllOut == m_clock.m_sysclk) {
            return factor - 2;
        }
    }
    while (true) {
    }
    return -1;
}
