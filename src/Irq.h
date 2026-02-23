#pragma once

#include "stm32f103x6.h"
#include <chrono>
#include <functional>
#include <thread>

static volatile uint32_t msTicks = 0;
extern "C" void SysTick_Handler();

void delay(uint32_t ms);

static std::function<void()> exti9_5callback;

extern "C" void EXTI9_5_IRQHandler();
extern "C" void EXTI2_IRQHandler();

extern "C" void sleep(uint32_t seconds);
extern "C" void usleep(uint32_t microseconds);

extern "C" void TIM1_UP_IRQHandler();
extern "C" void TIM2_IRQHandler();
extern "C" void TIM3_IRQHandler();
