#pragma once

#include <sys/time.h>

extern "C" unsigned int sleep(unsigned int seconds);
extern "C" int usleep(unsigned int microseconds);
extern "C" int _gettimeofday(struct timeval* tv, void* tz);
