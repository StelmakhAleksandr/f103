#include "Timer.h"

namespace {
IRQn_Type timerIrqFromInstance(TIM_TypeDef* instance)
{
    if (instance == TIM1) {
        return TIM1_UP_IRQn;
    }
    if (instance == TIM2) {
        return TIM2_IRQn;
    }
    if (instance == TIM3) {
        return TIM3_IRQn;
    }
    return TIM1_UP_IRQn;
}
}

Timer::Timer(TIM_TypeDef* instance)
    : m_instance(instance)
{
}

TimerBuilder Timer::builder(TIM_TypeDef* instance)
{
    return TimerBuilder(instance);
}

void Timer::start()
{
    m_instance->EGR = TIM_EGR_UG;
    m_instance->CR1 |= TIM_CR1_CEN;
}

void Timer::stop()
{
    m_instance->CR1 &= ~TIM_CR1_CEN;
}

void Timer::setPrescaler(uint16_t prescaler)
{
    m_instance->PSC = prescaler;
}

void Timer::setPeriod(uint32_t period)
{
    m_period = period;
    m_instance->ARR = period;
}

uint32_t Timer::getPeriod() const
{
    return m_period;
}

void Timer::setCompare(Channel channel, uint32_t compare)
{
    switch (channel) {
    case Channel::Ch1:
        m_instance->CCR1 = compare;
        break;
    case Channel::Ch2:
        m_instance->CCR2 = compare;
        break;
    case Channel::Ch3:
        m_instance->CCR3 = compare;
        break;
    case Channel::Ch4:
        m_instance->CCR4 = compare;
        break;
    }
}

void Timer::enablePeripheralClock(TIM_TypeDef* instance)
{
    if (instance == TIM1) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    } else if (instance == TIM2) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    } else if (instance == TIM3) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    }
}

bool Timer::isAdvancedTimer(TIM_TypeDef* instance)
{
    return instance == TIM1;
}

void Timer::configurePwmPins(const PwmChannelConfig& config)
{
    if (!config.enabled) {
        return;
    }

    auto setupAfPpPin = [](auto builder) {
        [[maybe_unused]] auto pin = builder
                                        .setMode(GpioMode::Output50Mhz)
                                        .setOutputMode(GpioOutputMode::AlternateFunctionPushPull)
                                        .build();
    };

    if (m_instance == TIM1) {
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

        PwmPinout pinout = config.pinout;
        if (pinout == PwmPinout::Auto) {
            pinout = PwmPinout::Tim1NoRemap;
        }

        AFIO->MAPR &= ~AFIO_MAPR_TIM1_REMAP;
        if (pinout == PwmPinout::Tim1PartialRemap) {
            AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_0;
        }

        if (config.channel == Channel::Ch1) {
            setupAfPpPin(PA8);
            if (config.enableComplementary) {
                if (pinout == PwmPinout::Tim1PartialRemap) {
                    setupAfPpPin(PA7);
                } else {
                    setupAfPpPin(PB13);
                }
            }
        } else if (config.channel == Channel::Ch2) {
            setupAfPpPin(PA9);
            if (config.enableComplementary) {
                if (pinout == PwmPinout::Tim1PartialRemap) {
                    setupAfPpPin(PB0);
                } else {
                    setupAfPpPin(PB14);
                }
            }
        } else if (config.channel == Channel::Ch3) {
            setupAfPpPin(PA10);
            if (config.enableComplementary) {
                if (pinout == PwmPinout::Tim1PartialRemap) {
                    setupAfPpPin(PB1);
                } else {
                    setupAfPpPin(PB15);
                }
            }
        } else if (config.channel == Channel::Ch4) {
            setupAfPpPin(PA11);
        }
        return;
    }

    if (m_instance == TIM2) {
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

        PwmPinout pinout = config.pinout;
        if (pinout == PwmPinout::Auto) {
            pinout = PwmPinout::Tim2NoRemap;
        }

        AFIO->MAPR &= ~AFIO_MAPR_TIM2_REMAP;
        if (pinout == PwmPinout::Tim2PartialRemap1) {
            AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_0;
        } else if (pinout == PwmPinout::Tim2PartialRemap2) {
            AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_1;
        } else if (pinout == PwmPinout::Tim2FullRemap) {
            AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP;
        }

        if (config.channel == Channel::Ch1) {
            if (pinout == PwmPinout::Tim2PartialRemap1 || pinout == PwmPinout::Tim2FullRemap) {
                setupAfPpPin(PA15);
            } else {
                setupAfPpPin(PA0);
            }
        } else if (config.channel == Channel::Ch2) {
            if (pinout == PwmPinout::Tim2PartialRemap1 || pinout == PwmPinout::Tim2FullRemap) {
                setupAfPpPin(PB3);
            } else {
                setupAfPpPin(PA1);
            }
        } else if (config.channel == Channel::Ch3) {
            if (pinout == PwmPinout::Tim2PartialRemap2 || pinout == PwmPinout::Tim2FullRemap) {
                setupAfPpPin(PB10);
            } else {
                setupAfPpPin(PA2);
            }
        } else if (config.channel == Channel::Ch4) {
            if (pinout == PwmPinout::Tim2PartialRemap2 || pinout == PwmPinout::Tim2FullRemap) {
                setupAfPpPin(PB11);
            } else {
                setupAfPpPin(PA3);
            }
        }
        return;
    }

    if (m_instance == TIM3) {
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

        PwmPinout pinout = config.pinout;
        if (pinout == PwmPinout::Auto) {
            pinout = PwmPinout::Tim3NoRemap;
        }

        AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP;
        if (pinout == PwmPinout::Tim3PartialRemap) {
            AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_0;
        } else if (pinout == PwmPinout::Tim3FullRemap) {
            AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP;
        }

        if (config.channel == Channel::Ch1) {
            if (pinout == PwmPinout::Tim3PartialRemap) {
                setupAfPpPin(PB4);
            } else if (pinout == PwmPinout::Tim3FullRemap) {
                setupAfPpPin(PC6);
            } else {
                setupAfPpPin(PA6);
            }
        } else if (config.channel == Channel::Ch2) {
            if (pinout == PwmPinout::Tim3PartialRemap) {
                setupAfPpPin(PB5);
            } else if (pinout == PwmPinout::Tim3FullRemap) {
                setupAfPpPin(PC7);
            } else {
                setupAfPpPin(PA7);
            }
        } else if (config.channel == Channel::Ch3) {
            if (pinout == PwmPinout::Tim3FullRemap) {
                setupAfPpPin(PC8);
            } else {
                setupAfPpPin(PB0);
            }
        } else if (config.channel == Channel::Ch4) {
            if (pinout == PwmPinout::Tim3FullRemap) {
                setupAfPpPin(PC9);
            } else {
                setupAfPpPin(PB1);
            }
        }
    }
}

void Timer::applyPwmChannelConfig(const PwmChannelConfig& config)
{
    if (!config.enabled) {
        return;
    }

    const uint32_t modeBits = (config.mode == PwmMode::Mode1) ? 0b110U : 0b111U;

    switch (config.channel) {
    case Channel::Ch1:
        m_instance->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M);
        m_instance->CCMR1 |= TIM_CCMR1_OC1PE;
        m_instance->CCMR1 |= (modeBits << TIM_CCMR1_OC1M_Pos);
        m_instance->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
        if (config.mainInverted) {
            m_instance->CCER |= TIM_CCER_CC1P;
        }
        m_instance->CCER |= TIM_CCER_CC1E;
        if (config.enableComplementary && isAdvancedTimer(m_instance)) {
            m_instance->CCER |= TIM_CCER_CC1NE;
            if (config.complementaryInverted) {
                m_instance->CCER |= TIM_CCER_CC1NP;
            }
        }
        break;

    case Channel::Ch2:
        m_instance->CCMR1 &= ~(TIM_CCMR1_CC2S | TIM_CCMR1_OC2M);
        m_instance->CCMR1 |= TIM_CCMR1_OC2PE;
        m_instance->CCMR1 |= (modeBits << TIM_CCMR1_OC2M_Pos);
        m_instance->CCER &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
        if (config.mainInverted) {
            m_instance->CCER |= TIM_CCER_CC2P;
        }
        m_instance->CCER |= TIM_CCER_CC2E;
        if (config.enableComplementary && isAdvancedTimer(m_instance)) {
            m_instance->CCER |= TIM_CCER_CC2NE;
            if (config.complementaryInverted) {
                m_instance->CCER |= TIM_CCER_CC2NP;
            }
        }
        break;

    case Channel::Ch3:
        m_instance->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_OC3M);
        m_instance->CCMR2 |= TIM_CCMR2_OC3PE;
        m_instance->CCMR2 |= (modeBits << TIM_CCMR2_OC3M_Pos);
        m_instance->CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
        if (config.mainInverted) {
            m_instance->CCER |= TIM_CCER_CC3P;
        }
        m_instance->CCER |= TIM_CCER_CC3E;
        if (config.enableComplementary && isAdvancedTimer(m_instance)) {
            m_instance->CCER |= TIM_CCER_CC3NE;
            if (config.complementaryInverted) {
                m_instance->CCER |= TIM_CCER_CC3NP;
            }
        }
        break;

    case Channel::Ch4:
        m_instance->CCMR2 &= ~(TIM_CCMR2_CC4S | TIM_CCMR2_OC4M);
        m_instance->CCMR2 |= TIM_CCMR2_OC4PE;
        m_instance->CCMR2 |= (modeBits << TIM_CCMR2_OC4M_Pos);
        m_instance->CCER &= ~TIM_CCER_CC4P;
        if (config.mainInverted) {
            m_instance->CCER |= TIM_CCER_CC4P;
        }
        m_instance->CCER |= TIM_CCER_CC4E;
        break;
    }

    setCompare(config.channel, config.compare);
}

void Timer::enableMainOutputIfNeeded(bool pwmConfigured)
{
    if (pwmConfigured && isAdvancedTimer(m_instance)) {
        m_instance->BDTR |= TIM_BDTR_MOE;
    }
}

TimerBuilder::TimerBuilder(TIM_TypeDef* instance)
    : m_instance(instance)
{
}

TimerBuilder& TimerBuilder::setPrescaler(uint16_t prescaler)
{
    m_prescaler = prescaler;
    return *this;
}

TimerBuilder& TimerBuilder::setPeriod(uint32_t period)
{
    m_period = period;
    return *this;
}

TimerBuilder& TimerBuilder::enableUpdateInterrupt(bool enabled)
{
    m_enableUpdateInterrupt = enabled;
    return *this;
}

TimerBuilder& TimerBuilder::enableAutoReloadPreload(bool enabled)
{
    m_enableAutoReloadPreload = enabled;
    return *this;
}

TimerBuilder& TimerBuilder::configurePwmChannel(Timer::Channel channel, uint32_t compare)
{
    return configurePwmChannel(channel, compare, Timer::PwmChannelOptions {});
}

TimerBuilder& TimerBuilder::configurePwmChannel(Timer::Channel channel,
    uint32_t compare,
    Timer::PwmChannelOptions options)
{
    const uint32_t index = static_cast<uint32_t>(channel);
    m_pwmChannels[index].enabled = true;
    m_pwmChannels[index].channel = channel;
    m_pwmChannels[index].compare = compare;
    m_pwmChannels[index].mode = options.mode;
    m_pwmChannels[index].mainInverted = options.mainInverted;
    m_pwmChannels[index].enableComplementary = options.enableComplementary;
    m_pwmChannels[index].complementaryInverted = options.complementaryInverted;
    m_pwmChannels[index].pinout = options.pinout;
    return *this;
}

Timer TimerBuilder::build()
{
    Timer timer(m_instance);

    Timer::enablePeripheralClock(m_instance);

    timer.m_instance->CR1 = 0U;
    timer.m_instance->CR2 = 0U;
    timer.m_instance->SMCR = 0U;
    timer.m_instance->DIER = 0U;

    timer.setPrescaler(m_prescaler);
    timer.setPeriod(m_period);

    if (m_enableAutoReloadPreload) {
        timer.m_instance->CR1 |= TIM_CR1_ARPE;
    }

    if (m_enableUpdateInterrupt) {
        timer.m_instance->DIER |= TIM_DIER_UIE;
        NVIC_EnableIRQ(timerIrqFromInstance(m_instance));
    }

    bool pwmConfigured = false;
    for (const auto& channelConfig : m_pwmChannels) {
        if (channelConfig.enabled) {
            pwmConfigured = true;
            timer.configurePwmPins(channelConfig);
            timer.applyPwmChannelConfig(channelConfig);
        }
    }

    timer.enableMainOutputIfNeeded(pwmConfigured);
    timer.m_instance->EGR = TIM_EGR_UG;

    return timer;
}
