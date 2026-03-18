#include "App.h"
#include <cstdio>

#include "rtos/Tasks.h"

void App::setup()
{
    m_clocks.emplace(ClocksBuilder()
                         .setHse(8_MHz)
                         .setSysclk(72_MHz)
                         .setPrescalers(AhbPrescaler::Div1, Apb1Prescaler::Div2, Apb2Prescaler::Div1)
                         .setAdcPrescaler(AdcPrescaler::Div6)
                         .setSysTick(1_kHz)
                         .build());

    m_ledPA12.emplace(PA12.setMode(GpioMode::Output10Mhz).build());
    m_buttonPb5.emplace(PB5
                            .setMode(GpioMode::Input)
                            .setInputMode(GpioInputMode::PullUpDown)
                            .setPull(GpioPull::Up)
                            .enableExti(GpioExtiTrigger::RisingAndFalling)
                            .build());

    Irq::instance().setExti5Callback([this]() {
        bool pressed = m_buttonPb5->isLow();
        if (pressed) {
            m_ledPA12->on();
        } else {
            m_ledPA12->off();
        }
    });

    m_pwmTimer.emplace(Timer::builder(TIM1)
                           .setPrescaler(m_clocks->pclk2() / 1000_kHz - 1U)
                           .setPeriod(100 - 1U)
                           .configurePwmChannel(Timer::Channel::Ch1,
                               90,
                               Timer::PwmChannelOptions(
                                   Timer::PwmMode::Mode1,
                                   false,
                                   true,
                                   true,
                                   Timer::PwmPinout::Tim1PartialRemap))
                           //    .configurePwmChannel(Timer::Channel::Ch2,
                           //        4_kHz,
                           //        Timer::PwmChannelOptions(
                           //            Timer::PwmMode::Mode1,
                           //            false,
                           //            true,
                           //            true,
                           //            Timer::PwmPinout::Tim1PartialRemap))
                           .build());

    m_baseTimer.emplace(Timer::builder(TIM3)
                            .setPrescaler(m_clocks->pclk1Timers() / 100_kHz - 1U)
                            .setPeriod(10_kHz / 2 - 1U)
                            .enableUpdateInterrupt(true)
                            .build());

    Irq::instance().setTim3Callback([this]() {
        m_adc1.startConversion();
    });

    m_usart1.setup(m_clocks->pclk2(), 115200);
    // m_commandParser.emplace(m_usart1, [this](const char* command) {
    //     printf("Received command: %s\r\n", command);
    // });
    m_adc1.setup([this](int value) {
        int percentage = value * 100 / 4095;
        if (m_adcPercentage != percentage) {
            m_adcPercentage = percentage;
            m_pwmTimer->stop();
            m_pwmTimer->setCompare(Timer::Channel::Ch1, percentage);
            m_pwmTimer->start();
            printf("ADC value: %d, percentage: %d%%\r\n", value, percentage);
        }
    });
}

void App::loop()
{
    using namespace std::chrono_literals;

    m_pwmTimer->start();
    m_baseTimer->start();

    makeTask([this]() {
        while (true) {
            std::this_thread::sleep_for(1s);
            printf("Hello, clocks =  %d \r\n", (int)m_clocks->sysclk());
        }
    },
        "Main",
        TaskStackSize::Large,
        TaskPriority::Low);

    vTaskStartScheduler();
    while (true) {
    }
}
