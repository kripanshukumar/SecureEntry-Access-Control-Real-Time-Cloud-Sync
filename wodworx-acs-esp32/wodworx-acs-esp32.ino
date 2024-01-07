#include "Variables.h"
#include "htmlPage.h"
#include "Internal_Storage.h"
#include "Storage.h"
#include "response.h"
#include "asyncWebserver.h"
#include "Wireless.h"
#include "RFID.h"
#include "SDK.h"
#include "LED.h"
#include "fingerprint.h"

uint64_t KeyOnRFID = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n\n================================================================================================================================\n");
  initLEDS();
  initInternalStorage();
  mutex = xSemaphoreCreateMutex();

  initStorage();
  listDir("/", 0);
  loadhtml();
  initWiFi();
  RequestKeys();
  //readFile(filename);
  loadKeysFromStorage();
  if (FingerPrintEnabled == 1) {
    initFingerPrintSensor();
  }
  initRFID();
  Serial.println("\n--------------------------------Device is Ready!---------------------------------");
  Serial.println("\nPlease Place your card on the RFID Scanner.....");

  xTaskCreate(
    LEDS_BUZZERS,     /* Task function. */
    "LED_BUZZERS",    /* String with name of task. */
    5000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    2,                /* Priority of the task. */
    NULL);            /* Task handle. */

  xTaskCreate(
    updateKeys,           /* Task function. */
    "UpdateKeys",         /* String with name of task. */
    10000,                /* Stack size in bytes. */
    NULL,                 /* Parameter passed as input of the task */
    4,                    /* Priority of the task. */
    NULL);                /* Task handle. */

  xTaskCreate(
    activateRelay,       /* Task function. */
    "Run Relay",         /* String with name of task. */
    1500,                /* Stack size in bytes. */
    NULL,                /* Parameter passed as input of the task */
    3,                   /* Priority of the task. */
    NULL);               /* Task handle. */

  xTaskCreate(
    reconnectThread,     /* Task function. */
    "Reconnect WiFi",    /* String with name of task. */
    5000,                /* Stack size in bytes. */
    NULL,                /* Parameter passed as input of the task */
    1,                   /* Priority of the task. */
    NULL);               /* Task handle. */


}

void loop() {
  ws.cleanupClients();
  KeyOnRFID = readRFID();
  if (FingerPrintEnabled == 1) {
    int ID_Fingerprint = getFingerPrintID();
    if (ID_Fingerprint > 0) {
      uint64_t tempKey = 0;
      bool acc = verifyPrint(ID_Fingerprint, &tempKey);
      eventsToServer(tempKey, acc);
    }
    enrol_on_button_push();
  }
  
  if (KeyOnRFID > 0) {
    eventsToServer(KeyOnRFID, verifyKey(KeyOnRFID));
    KeyOnRFID = 0;
    Serial.println("\nPlease Place your card on the RFID Scanner.....");
  }
  if (Primary_Request_Done != true && millis() - repeatRequestTimer >= 5000) {
    RequestKeys();
    repeatRequestTimer = millis();
  }
  if (millis() - timer >= 5000) {
    //initStorage();
    timer = millis();
    //    Serial.println("Update Stack: " + String(updateStack) + " Relay Stack: " + String(relayStack) + " LED Stack: " + String(ledStack));
    //    Serial.println("Total heap: " + String(ESP.getHeapSize()) + " Free heap: " + String(ESP.getFreeHeap()) + " Total PSRAM: " + String(ESP.getPsramSize()) + "Free PSRAM: " +  String(ESP.getFreePsram()));
    //    Serial.print("Size of total Buffer: "); Serial.println(sizeof(localKeys) * MAX_KEYS);
  }
  //Serial.println("In the loop.....");
  delay(10);
}
