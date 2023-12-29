// based on https://raphaelpralat.medium.com/example-of-json-rest-api-for-esp32-4a5f64774a05
// Chip is ESP32-D0WDQ6 (revision v1.0)
// Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None
// Crystal is 40MHz
// curl -i -X GET http://192.168.4.8/
// Should return:
// {"message":"Welcome"}

// Test /get-message route with a param
// curl -i -X GET http://192.168.4.8/get-message?message=HelloServer
// Should return:
// {"message":"HelloServer"}

// curl -i -d '{"message":"My sample data"}'  -H "Content-Type: application/json"  -X POST http://192.168.4.8/post-message
// Should return:
// {"message":"My sample data"}

// curl -i -d '{"message":"My sample data"}'  -H "Content-Type: application/json"  -X POST http://192.168.4.8/status-message
// Should return:
// {"give":"dataz"}

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "ESPAsyncWebServer.h"
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include <MIDI.h>
#include <HardwareSerial.h>
// #include <Arduino_JSON.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
//
const int sendRateMillis = 10;
const char *domainName = "razom"; // GATEWAY LOCAL DOMAIN NAME. WILL BE AVAILABLE AT http://VARIABLE-NAME.local
const char *ssid = "ESP-HUB";
const char *password = "Esphub666!";
//
#define LED_BUILTIN 2
// TASK HANDLES FOR MULTICORE EXECUTION
TaskHandle_t midiTask;
TaskHandle_t wifiTask;
// GLOBAL STATE VARIABLES
int midiTimer = 100; // midi output fps timer
int lastMillis = 0;  // midi output fps timer
int tolerance = 3;
int rollMapMax = 250;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
// PWM LED
const int PWMfreq = 5000;
const int PWMledChannel = 0;
const int PWMresolution = 8;
//
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi2);
AsyncWebServer server(80);
// CUSTOM DEFINED STRUCTURE TO ACCUMULATE LOCAL REPRESENTATION OF UI AND SENDER NODE DATA
typedef struct struct_nodeAccumulator
{
  // NODE PARAMS
  byte id = 1; // unique for each sender node
  // unsigned long idleTime = 0; //  time elapsed since a node has talked to the gateway
  int mode = 0; // node mapping mode. 0 = pitch/roll mapping
  // String MAC;
  byte fps = 10;
  bool active = true;
  int mapSourceMin = 0;
  int mapSourceMax = 300;
  byte mapTargetMin = 1;
  byte mapTargetMax = 127;

  // PITCH
  int pitchCH = 1;
  int pitchCC = 53;
  int pitchPrevMidiVal = 0;

  // ROLL
  int rollCH = 1;
  int rollCC = 117;
  int rollPrevMidiVal = 0;
  // ACCELERATION X
  int accelXCH = 1;
  int accelXCC = 117;
  int accelXPrevMidiVal = 0;
  // ACCELERATION Y
  int accelYCH = 1;
  int accelYCC = 117;
  int accelYPrevMidiVal = 0;
  // ACCELERATION Z
  int accelZCH = 1;
  int accelZCC = 117;
  int accelZPrevMidiVal = 0;
  // Z-KNOB
  int zKnobCH = 1;
  int zKnobCC = 117;
  int zKnobPrevMidiVal = 0;

  // SENSOR VALUES
  int anX = 1; // angle x
  int anY = 1; // angle y
  int anZ = 1; // angle z
  int acX = 1; // acceleration x
  int acY = 1; // acceleration y
  int acZ = 1; // acceleration z
  int gyX = 1; // gyro x
  int gyY = 1; // gyro y
  int gyZ = 1; // gyro z
  int aaX = 1; // acc angle x
  int aaY = 1; // acc angle y
} struct_nodeAccumulator;
// CUSTOM DEFINED STRUCTURE TO HOLD DATA RECEIVED FROM A SENDER NODE
typedef struct struct_nodeMsg
{
  byte id = 1; // unique for each sender node
  int anX = 1; // angle x
  int anY = 1; // angle y
  int anZ = 1; // angle z
  int acX = 1; // acceleration x
  int acY = 1; // acceleration y
  int acZ = 1; // acceleration z
  int gyX = 1; // gyro x
  int gyY = 1; // gyro y
  int gyZ = 1; // gyro z
  int aaX = 1; // acc angle x
  int aaY = 1; // acc angle y
} struct_nodeMsg;

// this data object holds data of an incoming message
struct_nodeMsg currMsg;
// holds data and status of from each node
struct_nodeAccumulator node0;
struct_nodeAccumulator node1;
struct_nodeAccumulator node2;
struct_nodeAccumulator node3;
// struct_nodeAccumulator node4;
// struct_nodeAccumulator node5;
// Create an array with all the structures
struct_nodeAccumulator nodesStruct[6] = {node0, node1, node2, node3};
// struct_nodeAccumulator nodesStruct[6] = {node0, node1, node2, node3, node4, node5};
//

//
#include "espnowFn.h"
#include "serverFn.h"
#include "btnFn.h"
#include "initFn.h"

void midiTaskFunction(void *pvParameters)
{
  for (;;)
  { // MIDI CALCULATE AND OUT
    // btnRoutine();
    currentMillis = millis();
    if (currentMillis - previousMillis >= sendRateMillis) // IF sendRateMillis MILLISECONDS HAVE PASSED
    {
      previousMillis = currentMillis;
      for (int i = 0; i < 4; i++) // REPEAT FOR ALL 4 NODES
      {
        // Serial.print("NODE: ");
        // Serial.println(i);
        if (nodesStruct[i].active) // NODE ACTIVED BY USER
        {
          // Serial.print("midi timer fired. active in mode: ");
          // Serial.println(nodesStruct[i].mode);

          switch (nodesStruct[i].mode) // CALCULATE MIDI OUTPUT ACCORDING TO CURRENT OPERATION MODE
          {
          case 0: // PITCH/ROLL  MODE
          {       // Serial.print("ROLL/PITCH EVENT FIRED AT Core");
                  // Serial.println(xPortGetCoreID());
            //       // ROLL MIDI ROUTINE
            // int absAaxRawValue = abs(nodesStruct[i].aaX);
            // if (absAaxRawValue > nodesStruct[i].mapSourceMin) // if greaten than hysteresis tolerance
            // {
            //   int currMidiVal = map(absAaxRawValue,                 // VALUE
            //                         nodesStruct[i].mapSourceMin,    // FROM MIN
            //                         nodesStruct[i].mapSourceMax,    // FROM MAX
            //                         nodesStruct[i].mapTargetMin,    // TO MIN
            //                         nodesStruct[i].mapTargetMax);   // TO MAX
            //                                                         // CONSTRAIN in case ofmap overshooting the max value
            //   currMidiVal = constrain(currMidiVal,                  // VALUE
            //                           nodesStruct[i].mapTargetMin,  // MIN
            //                           nodesStruct[i].mapTargetMax); // MAX
            //   // PROCEED  IF NEW MIDI VALUE DIFFERS FROM LAST
            //   if (currMidiVal != nodesStruct[i].rollPrevMidiVal)
            //   {
            //     // SEND MIDI CONTROLL CHANGE!;)
            //     midi2.sendControlChange(nodesStruct[i].rollCC, currMidiVal, nodesStruct[i].rollCH);
            //     nodesStruct[i].rollPrevMidiVal = currMidiVal; // update last known midi roll value
            //     // SERIAL REPORT
            //     // Serial.print("MIDI THREAD AT Core");
            //     // Serial.println(xPortGetCoreID());
            //     Serial.print(">Roll MIDI Value: ");
            //     Serial.println(currMidiVal);
            //     // ledcWrite(PWMledChannel, currRollMidiVal * 2);
            //   }
            // }
            // PITCH MIDI ROUTINE
            int absAayRawValue = abs(nodesStruct[i].aaY);
            if (absAayRawValue > nodesStruct[i].mapSourceMin) // if greaten than hysteresis tolerance
            {
              int currMidiVal = map(absAayRawValue,                 // VALUE
                                    nodesStruct[i].mapSourceMin,    // FROM MIN
                                    nodesStruct[i].mapSourceMax,    // FROM MAX
                                    nodesStruct[i].mapTargetMin,    // TO MIN
                                    nodesStruct[i].mapTargetMax);   // TO MAX
                                                                    // CONSTRAIN in case ofmap overshooting the max value
              currMidiVal = constrain(currMidiVal,                  // VALUE
                                      nodesStruct[i].mapTargetMin,  // MIN
                                      nodesStruct[i].mapTargetMax); // MAX
              // PROCEED  IF NEW MIDI VALUE DIFFERS FROM LAST
              if (currMidiVal != nodesStruct[i].pitchPrevMidiVal)
              {
                // SEND MIDI CONTROLL CHANGE!;)
                midi2.sendControlChange(nodesStruct[i].pitchCC, currMidiVal, nodesStruct[i].pitchCH);
                nodesStruct[i].pitchPrevMidiVal = currMidiVal; // update last known midi pitch value
                // SERIAL REPORT
                // Serial.print("MIDI THREAD AT Core");
                // Serial.println(xPortGetCoreID());
                Serial.print(">pitch MIDI Value: ");
                Serial.println(currMidiVal);
                // ledcWrite(PWMledChannel, currpitchMidiVal * 2);
              }
              // midi2.sendControlChange(7, 64, 1); // ControlNumber(1=pitch bend 7=volume), ControlValue, Channel
            }
          }
          break;
          case 1: // ACCELERATION MODE
          {
            // Serial.print("ACCELERATION EVENT FIRED AT Core");
            // Serial.println(xPortGetCoreID());
            // ACCELERATION MIDI ROUTINE
            int absAcxRawValue = abs(nodesStruct[i].acX);     // use absolute value for both directions
            if (absAcxRawValue > nodesStruct[i].mapSourceMin) // if greaten than hysteresis tolerance
            {
              int currMidiVal = map(absAcxRawValue,                 // VALUE
                                    nodesStruct[i].mapSourceMin,    // FROM MIN
                                    nodesStruct[i].mapSourceMax,    // FROM MAX
                                    nodesStruct[i].mapTargetMin,    // TO MIN
                                    nodesStruct[i].mapTargetMax);   // TO MAX
                                                                    // CONSTRAIN in case ofmap overshooting the max value
              currMidiVal = constrain(currMidiVal,                  // VALUE
                                      nodesStruct[i].mapTargetMin,  // MIN
                                      nodesStruct[i].mapTargetMax); // MAX
              // PROCEED  IF NEW MIDI VALUE DIFFERS FROM LAST
              if (currMidiVal != nodesStruct[i].accelXPrevMidiVal)
              {
                // SEND MIDI CONTROLL CHANGE!;)
                midi2.sendControlChange(nodesStruct[i].accelXCC, currMidiVal, nodesStruct[i].accelXCH);
                nodesStruct[i].accelXPrevMidiVal = currMidiVal; // update last known midi pitch value
                // SERIAL REPORT
                // Serial.print("MIDI THREAD AT Core");
                // Serial.println(xPortGetCoreID());
                Serial.print(">ACCEL X MIDI Value: ");
                Serial.println(currMidiVal);
                // ledcWrite(PWMledChannel, currpitchMidiVal * 2);
              }
              // midi2.sendControlChange(7, 64, 1); // ControlNumber(1=pitch bend 7=volume), ControlValue, Channel
            }
          }
          break;
          case 2: // Z-KNOB  MODE
          {
            // Serial.print("Z-KNOB EVENT FIRED AT Core");
            // Serial.println(xPortGetCoreID());
            // Z-KNOB MIDI ROUTINE
            int absAnzRawValue = abs(nodesStruct[i].anZ);
            if (absAnzRawValue > nodesStruct[i].mapSourceMin) // if greaten than hysteresis tolerance
            {
              int currMidiVal = map(absAnzRawValue,                 // VALUE
                                    nodesStruct[i].mapSourceMin,    // FROM MIN
                                    nodesStruct[i].mapSourceMax,    // FROM MAX
                                    nodesStruct[i].mapTargetMin,    // TO MIN
                                    nodesStruct[i].mapTargetMax);   // TO MAX
                                                                    // CONSTRAIN in case ofmap overshooting the max value
              currMidiVal = constrain(currMidiVal,                  // VALUE
                                      nodesStruct[i].mapTargetMin,  // MIN
                                      nodesStruct[i].mapTargetMax); // MAX
              // PROCEED  IF NEW MIDI VALUE DIFFERS FROM LAST
              if (currMidiVal != nodesStruct[i].zKnobPrevMidiVal)
              {
                // SEND MIDI CONTROLL CHANGE!;)
                midi2.sendControlChange(nodesStruct[i].zKnobCC, currMidiVal, nodesStruct[i].zKnobCH);
                nodesStruct[i].zKnobPrevMidiVal = currMidiVal; // update last known midi pitch value
                // SERIAL REPORT
                // Serial.print("MIDI THREAD AT Core");
                // Serial.println(xPortGetCoreID());
                Serial.print(">Z KNOB MIDI Value: ");
                Serial.println(currMidiVal);
                // ledcWrite(PWMledChannel, currpitchMidiVal * 2);
              }
              // midi2.sendControlChange(7, 64, 1); // ControlNumber(1=pitch bend 7=volume), ControlValue, Channel
            }
          }
          break;
          default:
          {
          }
          break;
          }
        }
      }
    }
  }
}

void setup()
{
  // KORG VOLCA KEYS @CH1 DEFAULTS
  nodesStruct[0].pitchCC = 44;  // cutoff
  nodesStruct[0].rollCC = 44;   // cutoff
  nodesStruct[0].accelXCC = 44; // cutoff
  nodesStruct[0].zKnobCC = 44;  // cutoff
  // nodesStruct[0].mode = 2;           // start as z knob
  nodesStruct[0].mapSourceMax = 170; // 1000 for a good amout of rotation
  nodesStruct[0].mapTargetMin = 6;   // not completly closed
  nodesStruct[0].mapTargetMax = 110; // not completly open
  //
  nodesStruct[1].pitchCC = 42;  // detune
  nodesStruct[1].rollCC = 42;   // detune
  nodesStruct[1].accelXCC = 42; // detune
  nodesStruct[1].zKnobCC = 42;  // detune
  // nodesStruct[1].mode = 0;          // start as pitch/roll
  nodesStruct[1].mapTargetMin = 6;  // not completly closed
  nodesStruct[1].mapTargetMax = 65; // not completly open
  //
  nodesStruct[2].pitchCC = 45;       // vcf eg int
  nodesStruct[2].rollCC = 45;        // vcf eg int
  nodesStruct[2].accelXCC = 45;      // vcf eg int
  nodesStruct[2].zKnobCC = 45;       // vcf eg int
  nodesStruct[2].mode = 1;           // start as accel
  nodesStruct[2].mapSourceMax = 170; // 1000 for a good amout of rotation
  nodesStruct[2].mapTargetMin = 6;   // not completly closed
  nodesStruct[2].mapTargetMax = 110; // not completly open
  //
  nodesStruct[3].pitchCC = 53;       // delay feedback
  nodesStruct[3].rollCC = 53;        // delay feedback
  nodesStruct[3].accelXCC = 53;      // delay feedback
  nodesStruct[3].zKnobCC = 53;       // delay feedback
  nodesStruct[3].mode = 0;           // start as pitch/roll
  nodesStruct[3].mapTargetMin = 2;   // not completly closed
  nodesStruct[3].mapTargetMax = 100; // not completly open
  //
  initSerial();
  initMidi();
  initBtn(); // ACTIVATE PULLUPS AT  BUTTON PINS
  initLED();
  initFS(); // starts SPIFFileSystem
  initWifi();
  initEspNow();
  initDNS();
  initServerRoutes(); // sets up server routes and request handlers and starts the server
  xTaskCreatePinnedToCore(
      midiTaskFunction, /* Task function. */
      "midiTask",       /* name of task. */
      10000,            /* Stack size of task */
      NULL,             /* parameter of the task */
      10,               /* priority of the task */
      &midiTask,        /* Task handle to keep track of created task */
      1);               // pin task to core 0, aka Protocol Core or PRO CPU or core 1 Application Core or APP CPU
  delay(500);           // wait is needed

  Serial.println("SETUP FINISHED");
  Serial.println("");
}
void loop()
{
}