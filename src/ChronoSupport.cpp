#include "ChronoSupport.h"

#include "core/Irq.h"
#include "rtos/Tasks.h"

extern "C" int clock_gettime(clockid_t clockId, struct timespec* ts)
{
    if (ts == nullptr) {
        return -1;
    }

    (void)clockId;

    TickType_t ticks = 0;
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        ticks = xTaskGetTickCount();
    } else {
        ticks = 0;
    }

    const uint64_t ms = static_cast<uint64_t>(ticks) * 1000ULL / configTICK_RATE_HZ;
    ts->tv_sec = static_cast<time_t>(ms / 1000ULL);
    ts->tv_nsec = static_cast<long>((ms % 1000ULL) * 1000000ULL);

    return 0;
}

static void busyWaitMicroseconds(unsigned int microseconds)
{
    const uint32_t cyclesPerMicrosecond = SystemCoreClock / 1000000U;
    const uint32_t totalCycles = cyclesPerMicrosecond * microseconds;
    for (volatile uint32_t i = 0; i < (totalCycles / 4U); i = i + 1) {
        __asm volatile("nop");
    }
}

extern "C" unsigned int sleep(unsigned int seconds)
{
    if (seconds == 0U) {
        taskYIELD();
        return 0;
    }

    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        busyWaitMicroseconds(seconds * 1000000UL);
        return 0;
    }

    vTaskDelay(pdMS_TO_TICKS(seconds * 1000UL));
    return 0;
}

extern "C" int usleep(unsigned int microseconds)
{
    if (microseconds == 0U) {
        return 0;
    }

    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        busyWaitMicroseconds(microseconds);
        return 0;
    }

    if (microseconds < 1000U) {
        busyWaitMicroseconds(microseconds);
        return 0;
    }

    const unsigned int milliseconds = (microseconds + 999U) / 1000U;
    vTaskDelay(pdMS_TO_TICKS(milliseconds));
    return 0;
}

extern "C" int _gettimeofday(struct timeval* timeValue, void*)
{
    if (timeValue == nullptr) {
        return -1;
    }

    TickType_t ticks = 0;
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        ticks = xTaskGetTickCount();
    }

    const uint64_t milliseconds = static_cast<uint64_t>(ticks) * 1000ULL / static_cast<uint64_t>(configTICK_RATE_HZ);
    timeValue->tv_sec = static_cast<time_t>(milliseconds / 1000ULL);
    timeValue->tv_usec = static_cast<suseconds_t>((milliseconds % 1000ULL) * 1000ULL);

    return 0;
}
