#include "Hooks.h"

extern "C" void vApplicationStackOverflowHook(TaskHandle_t, char*)
{
    taskDISABLE_INTERRUPTS();
    for (;;) {
    }
}

extern "C" void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for (;;) {
    }
}