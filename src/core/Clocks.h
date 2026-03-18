#pragma once

#include <cstdint>

constexpr unsigned long long operator"" _hz(unsigned long long val)
{
    return val;
}

constexpr unsigned long long operator"" _kHz(unsigned long long val)
{
    return val * 1000;
}

constexpr unsigned long long operator"" _MHz(unsigned long long val)
{
    return val * 1000000;
}

enum class AhbPrescaler : uint32_t
{
    Div1 = 1,
    Div2 = 2,
    Div4 = 4,
    Div8 = 8,
    Div16 = 16,
    Div64 = 64,
    Div128 = 128,
    Div256 = 256,
    Div512 = 512
};

enum class Apb1Prescaler : uint32_t
{
    Div1 = 1,
    Div2 = 2,
    Div4 = 4,
    Div8 = 8,
    Div16 = 16
};

enum class Apb2Prescaler : uint32_t
{
    Div1 = 1,
    Div2 = 2,
    Div4 = 4,
    Div8 = 8,
    Div16 = 16
};

enum class AdcPrescaler : uint32_t
{
    Div2 = 2,
    Div4 = 4,
    Div6 = 6,
    Div8 = 8,
};

class ClocksBuilder;

class Clocks
{
    friend class ClocksBuilder;

public:
    unsigned long long pclk1() const;
    unsigned long long hclk() const;
    unsigned long long pclk2() const;
    unsigned long long sysclk() const;
    unsigned long long pclk1Timers() const;

private:
    Clocks() = default;
    unsigned long long m_hclk;
    unsigned long long m_pclk1;
    unsigned long long m_pclk2;
    unsigned long long m_sysclk;
    unsigned long long m_hse = 0_MHz;

    void runHse();
    void runHsi();
    void runPll();
    void runPllclk();
    void setupPll(uint32_t mul);
    void setupBusPrescalers(AhbPrescaler ahb, Apb1Prescaler apb1, Apb2Prescaler apb2);
    void setFlashLatency();
    void switchSysclkToHsi();
    void disablePll();
    void enableFlashPrefetchAndHalfCycle();
    void setupAdcPrescaler(AdcPrescaler adc);
};

class ClocksBuilder
{
public:
    ClocksBuilder& setSysclk(unsigned long long);
    ClocksBuilder& setHse(unsigned long long);
    ClocksBuilder& setPrescalers(AhbPrescaler ahb, Apb1Prescaler apb1, Apb2Prescaler apb2);
    ClocksBuilder& setAdcPrescaler(AdcPrescaler adc);
    ClocksBuilder& setSysTick(uint32_t tickRateHz = 1000U);
    Clocks build();

private:
    uint32_t calculatePllMul();
    Clocks m_clock;
    AhbPrescaler m_ahb = AhbPrescaler::Div1;
    Apb1Prescaler m_apb1 = Apb1Prescaler::Div2;
    Apb2Prescaler m_apb2 = Apb2Prescaler::Div1;
    AdcPrescaler m_adc = AdcPrescaler::Div6;
    bool m_configureSysTick = false;
    uint32_t m_sysTickHz = 1000U;
};