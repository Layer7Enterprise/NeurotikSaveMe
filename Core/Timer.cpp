#include "Timer.h"

#ifdef OSX
static void (*millisecondCallback)() = NULL;
static float last_read = 0;
static float num_ticks = 0;  //For tracking skew

void Proxy(void *) {
  millisecondCallback();
  ++num_ticks;
}

static pthread_t skewTracker;
void *SkewTracker(void *) {
  while (1) {
    printf("Skew %f%%\n", (num_ticks - last_read) / last_read * 100.0);

    //Attempt to sample
    last_read = num_ticks;
    usleep(1000 * 1000);
    last_read += 1000;
  }

}

void OnMillisecond(void (*callback)()) {
  millisecondCallback = callback;

  dispatch_queue_t queue = dispatch_get_main_queue();
  dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);

  if (timer) {
    dispatch_source_set_timer(timer, 0, 1000 * 1000 , 1000 * 1000);
    dispatch_source_set_event_handler_f(timer, Proxy);
    dispatch_resume(timer);
  } else {
    fprintf(stderr, "Couldn't start millisecond timer\n");
    exit(EXIT_FAILURE);
  }

  pthread_create(&skewTracker, NULL, SkewTracker, NULL);
}

#endif

