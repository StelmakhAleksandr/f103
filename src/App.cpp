#include "App.h"

void App::setup()
{
    setupSysTick();
    ledPA12.setup();
    buttonPB5.setup();

    test = [this]() {
        ledPA12.toggle();
    };

    buttonPB5.setCallback([this](bool pressed) {
        if (pressed) {
            ledPA12.on();
        } else {
            ledPA12.off();
        }
    });
}

void App::loop()
{
    while (true) {
        // test();
        delay(1000);
    }
}

void App::setupSysTick()
{
    SysTick->VAL = 0U;
    SysTick->LOAD = 8000000U / 1000U - 1U;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
}