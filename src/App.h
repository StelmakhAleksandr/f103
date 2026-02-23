#pragma once

#include "Button1.h"
#include "Irq.h"
#include "Led1.h"

class App
{
public:
    void setup();
    void loop();

private:
    void setupSysTick();
    Led1 ledPA12;
    std::function<void()> test;
};