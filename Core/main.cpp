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
  RunUnits();
  return 0;
}
