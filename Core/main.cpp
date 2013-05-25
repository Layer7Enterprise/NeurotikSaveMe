#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <string>
#include <time.h>
#include <vector>
#include <sys/time.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <dispatch/dispatch.h>
#include "Net.h"
#include "Schema.h"
#include "Params.h"
#include "GetNet.h"
#include "CoreManager.h"
#include "Timer.h"

/*#define SCHEMA_FILE "config/schema.txt"*/
/*#define NET_FILE "config/net.txt"*/
#define NET_IP "224.4.5.6"
#define NET_PORT 3000  //Input = 3000, Output = 3001

#include "test/Units.h"

int main() {
  //system("ps -x | grep rcv_data | xargs -n 1 | head -n 1 | xargs kill");
  //sleep(1);

  //Setup network
  /*NetRcvBegin(NET_IP, NET_PORT, CoreOnImpulse);*/
  //NetSendBegin(NET_IP, NET_PORT+1);

  //FILE *pipe = popen("ruby ./test/utility/rcv.rb", "r");
  //if (pipe <= 0) {
    //fprintf(stderr, "\nUnit test could not open send_data.rb\n");
    //exit(EXIT_FAILURE);
  //}

  //NetSend("101", 3);

  //char output[100];
  //fgets(output, sizeof(output), pipe);
  /*puts(output);*/

  RunUnits();
  return 0;
}
