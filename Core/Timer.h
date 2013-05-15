#ifndef TIMER_H_
#define TIMER_H_

#define OSX

#ifdef OSX
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <dispatch/dispatch.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//Call the callback function every millisecond and skew checking
void OnMillisecond(void (*callback)());

#endif
