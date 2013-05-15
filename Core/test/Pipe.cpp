#include "Pipe.h"

#define RANDOM_CHAR() (0x45 + (rand() % 20))
std::queue<UnitMessageInfo_t> messages;

sem_t *semaphore = NULL;
std::list<sem_t *> semaphores;
void BeginUnitPipe() {
  int currentType = 0;

  //Create a semaphore for each message type
  for (int i = 0; i < NUM_UNIT_MESSAGE_TYPES; ++i) {
    //Generate a random name
    srand(time(NULL));
    const int randomNameLen = 20;
    static char randomName[randomNameLen];
    for (int i = 0; i < randomNameLen; ++i)
      randomName[i] = RANDOM_CHAR();
    semaphores[currentType] = sem_open(randomName, O_CREAT, O_RDWR, 0);

    if (!semaphore) {
      fprintf(stderr, "Could not create semaphore: error code %d", errno);
      exit(EXIT_FAILURE);
    }
  }
}

void SendUnitMessage(UnitMessageType_t type, const unsigned char *msg, int len) {
  //Copy into a message
  UnitMessageInfo_t info;
  info.type = type;
  memcpy(&info.msg, msg, len);
  info.len = len;

  messages.push(info);

  //Announce that there is an open semaphore
  sem_post(semaphore);
}

UnitMessageInfo_t GetUnitMessage(UnitMessageType_t type) {
  //Block until we have a message
  if (sem_wait(semaphore) < 0) {
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

  UnitMessageInfo_t message = messages.back();
  messages.pop();
  
  return messages.back();
}
