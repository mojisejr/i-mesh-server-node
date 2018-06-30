#include "painlessMesh.h"

#define   MESH_PREFIX     "chamber1"
#define   MESH_PASSWORD   "chamber123"
#define   MESH_PORT       5555

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
// Prototype
void receivedCallback( uint32_t from, String &msg );


// Send my ID every 10 seconds to inform others
Task ServerTask(10000, TASK_FOREVER, []() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& msg = jsonBuffer.createObject();
    msg["topic"] = "Server";
    msg["nodeId"] = mesh.getNodeId();
    msg["name"] = "iMS";

    String str;
    msg.printTo(str);
    mesh.sendBroadcast(str);

    // log to serial
    msg.printTo(Serial);
    Serial.printf("\n");
});

void setup() {
  Serial.begin(115200);
    
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE | DEBUG ); // all types on
  //mesh.setDebugMsgTypes( ERROR | CONNECTION | SYNC | S_TIME );  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes( ERROR | DEBUG | CONNECTION | S_TIME );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);

  mesh.onNewConnection([](size_t nodeId) {
    Serial.printf("New Connection %u\n", nodeId);
  });

  mesh.onDroppedConnection([](size_t nodeId) {
    Serial.printf("Dropped Connection %u\n", nodeId);
  });

  // Add the task to the your scheduler
  userScheduler.addTask(ServerTask);
  ServerTask.enable();
}

void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("SERVER: Received from %u msg=%s\n", from, msg.c_str());
}
