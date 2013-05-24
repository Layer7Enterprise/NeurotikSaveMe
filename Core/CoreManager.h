#ifndef CORE_MANAGER_H_
#define CORE_MANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <pthread.h>
#include <CCup.h>
#include "Params.h"
#include "Timer.h"

//Start the core
void CoreBegin(Params_t *params);

//Core millisecond tick
void CoreTick();

//Current input from network, etc.
void CoreOnImpulse(const char *impulse);

#endif
