#include "Timer.h"

static float last_read = 0;
static float num_ticks = 0;  //For tracking skew

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

#ifdef OSX
static void (*millisecondCallback)() = NULL;

void Proxy(void *) {
  millisecondCallback();
  ++num_ticks;
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

void HandleEvents() {
  dispatch_main();
}

//No need to setup
void SetupTimer() {
  return;
}

#endif

#ifdef LINUX
static struct event_base *base;
static void (*timer_callback)() = NULL;
static struct event millisecond_event;

void Proxy(int a, short b, void *c) {
  timer_callback();
  ++num_ticks;
}

void OnMillisecond(void (*callback)()) {
  timer_callback = callback;

  event_set(&millisecond_event, 0, EV_PERSIST, Proxy, NULL);
  struct timeval time;
  time.tv_sec = 0;
  time.tv_usec = 1000;
  event_base_set(base, &millisecond_event);
  evtimer_add(&millisecond_event, &time);
  pthread_create(&skewTracker, NULL, SkewTracker, NULL);
}

void SetupTimer() {
  base = event_base_new();
}

void HandleEvents() {
  event_base_dispatch(base);
}

#endif
