#include "Pipe.h"

#define RANDOM_CHAR() (0x45 + (rand() % 20))
static std::vector<std::queue<UnitMessageInfo_t> *> messages;

static std::vector<sem_t *> semaphores;
void BeginUnitPipe() {
  srand(time(NULL));

  //Create a semaphore for each message type
  for (int i = 0; i < NUM_UNIT_MESSAGE_TYPES; ++i) {
    //Generate a random name
    const int randomNameLen = 20;
    static char randomName[randomNameLen];
    randomName[19] = 0;
    for (int x = 0; x < randomNameLen-1; ++x)
      randomName[x] = RANDOM_CHAR();
    semaphores.push_back(sem_open(randomName, O_CREAT, O_RDWR, 0));

    if (!semaphores[i]) {
      fprintf(stderr, "Could not create semaphore: error code %d", errno);
      exit(EXIT_FAILURE);
    }

    //Create a queue for each message type
    messages.push_back(new std::queue<UnitMessageInfo_t>());
  }
}

void SendUnitMessage(UnitMessageType_t type, const unsigned char *msg, int len) {
  //Copy into a message
  UnitMessageInfo_t info;
  info.type = type;
  memcpy(&info.msg, msg, len);
  info.len = len;

  messages[type]->push(info);

  //Announce that there is an open semaphore
  sem_post(semaphores[type]);
}

UnitMessageInfo_t GetUnitMessage(UnitMessageType_t type) {
  //Block until we have a message
  if (sem_wait(semaphores[type]) < 0) {
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

  UnitMessageInfo_t message = messages[type]->back();
  messages[type]->pop();
  
  return message;
}
