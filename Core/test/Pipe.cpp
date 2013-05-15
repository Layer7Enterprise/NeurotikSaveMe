#include "Pipe.h"

sem_t semaphore;
void BeginUnitPipe() {
  int res = sem_init(&semaphore, 0, 0);

  if (res < 0) {
    fprintf(stderr, "Could not create semaphore: error code %d", errno);
    exit(EXIT_FAILURE);
  }
}

void SendToUnits(UnitMessage_t type, const unsigned char *msg, int len) {
  //Copy into a message
  UnitMessageInfo_t info;
  info.type = type;
  memcpy(&info.msg, msg, len);
  info.len = len;

  //Announce that there is an open semaphore
  sem_post(&semaphore);
}

void GetUnitMessage(UnitMessage_t *message) {
  if (sem_wait(&semaphore) < 0) {
    if (errno == EINVAL) {
      fprintf(stderr, "Could not wait on semaphore: invalid semaphore");
    } else if (errno == EDEADLK) {
      fprintf(stderr, "Could not wait on semaphore: deadlock condition");
    } else if (errno == EINTR) {
      fprintf(stderr, "Semaphore interrupted: not handling this case");
    } else {
      fprintf(stderr, "Semaphore interrupted: unknown error %d\n", errno);
    }

    exit(EXIT_FAILURE);
  }
}
