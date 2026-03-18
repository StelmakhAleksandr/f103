#pragma once

#include "Tasks.h"

extern "C" void vApplicationStackOverflowHook(TaskHandle_t, char*);
extern "C" void vApplicationMallocFailedHook(void);