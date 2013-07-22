#ifndef TIMER_H_
#define TIMER_H_

#include <event.h>
#include <unistd.h>

//#define OSX
#define LINUX

#ifdef OSX
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <dispatch/dispatch.h>
#endif

#ifdef LINUX
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//Setup event system
void SetupTimer();

//Main timer loop
void HandleEvents();

//Call the callback function every millisecond and skew checking
void OnMillisecond(void (*callback)());

#endif
