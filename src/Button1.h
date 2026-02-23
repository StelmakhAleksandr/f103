#pragma once

#include "Irq.h"

class Button1
{
public:
    void setup();
    bool pressed() const;
    void setCallback(std::function<void(bool)> callback);
    void handleInterrupt();

private:
    std::function<void(bool)> callback;
};

extern Button1 buttonPB5;