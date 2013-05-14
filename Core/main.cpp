#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <string>
#include "Net.h"
#include "Schema.h"
#include "Params.h"
#include "GetNet.h"

#define SCHEMA_FILE "config/schema.txt"
#define NET_FILE "config/net.txt"

#include "test/Units.h"

int main() {
  RunUnits();

  //Load schema (Port, name, etc)
  Schema_t schema;
  GetSchema(SCHEMA_FILE, &schema);

  //Load params (net list, large chunk of memory, etc)
  Params_t params;
  params.neuronNameToLocation = new std::map<std::string, int>();
  params.neuronLocationToName = new std::map<int, std::string>();
  GetNet(NET_FILE, schema, &params);

  /*NetSendBegin("224.4.5.6", 3000);*/
  //for (int i = 0; i < 100; ++i) 
    /*NetSend("test", 4);*/


  return 0;
}
