#include "FS.h"
#include "SD.h"
#include "SPIFFS.h"
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Fingerprint.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//====================================================================================================================================================//


#pragma pack(1)

#define NO_OF_FP_SLOTS 1000
#define FingerPrintEnabled 1             //Enable or disable finger print

#define UPDATE_INTERVAL     3000         // This defines the interval between two consecutive update request mad by ESP32. The value is in milliseconds.
#define RESTART_ENABLE      0            // Set to 1 if you want to attempt reconnection if WiFi is not connected in 1st Attempt. Else keep it 0
#define BEEP_DURATION       200          // This refers to the buzzer on time. (In milliseconds).
#define GREEN_LED_ON_TIME   2000         // This is the on time for green led when ever the access is granted. (In milliseconds)
#define RED_LED_ON_TIME     200          // As the red light truns on and off a total of 3 cycles, each cycale's half time is give by this definition. (in milliseconds)

#define SD_CS 32

#define MAX_KEYS 1000                    //Max Keys which system can accomodate
#define MAX_UPDATE_KEYS 500              //Max Update Keys which can be accomodate on each update request

#define RFID_RST_PIN         2           // Configurable, see typical pin layout above
#define RFID_SS_PIN          5            // Configurable, see typical pin layout above

#define BUZZER             33
#define ACCESS_LED_RED     25
#define ACCESS_LED_GREEN   26
#define NETWORK_LED        27
#define ORANGE_LED         15
#define ENROLL_BUTTON      34

#define RelayPin        12                // Relay Signal Output pin
#define RELAY_DELAY     3000              //Time for which the relay turns on. (in Milliseconds)

#define FORMAT_SPIFFS_IF_FAILED true


#define mySerial Serial2
#define RXD2 16
#define TXD2 17

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);   // Create MFRC522 instance

SPIClass spiSD(HSPI);

//WebServer server(80);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
//====================================================================================================================================================//
String ssid = "";
String password = "";
String api_key = "Kr7YX6ApS6GGGpc3";
String unique_id = "4";

String espSSID = "WODWORX_0000001";
String espPassword = "J5GrwN2r";

const char* filePassword = "access";
const char* host = "websettings";
const char* www_username = "admin";
const char* www_password = "admin";

// allows you to set the realm of authentication Default:"Login Required"
const char* www_realm = "Custom Auth Realm";
// the Content of the HTML response in case of Unautherized Access Default:empty
String authFailResponse = "Authentication Failed";

const char* accessEvent = "https://api.wodworx.com/access/v1/access-event";
const char* listAccessKeys = "https://api.wodworx.com/access/v1/access-key/list";
const char* checkChanges = "https://api.wodworx.com/access/v1/access-key/can-update?timestamp=";
const char* listKeyChanges = "https://api.wodworx.com/access/v1/access-key/changes?timestamp=";
const char* sendFingerprint = "https://api.wodworx.com/access/v1/access-fingerprint/create";


const char* filename = "/keys.txt";
String tempUploadedFile;

uint64_t localkeys[MAX_KEYS] = {0};
uint64_t delKeys[MAX_UPDATE_KEYS] = {0};
uint64_t addKeys[MAX_UPDATE_KEYS] = {0};
uint64_t deletionIndex[MAX_UPDATE_KEYS] = {0};
uint8_t takenFingerPos[NO_OF_FP_SLOTS + 1] = {0};
uint8_t deleteFingerPos[NO_OF_FP_SLOTS + 1] = {0};

//char* index_html;
int keysAvailable = 0;
int toDelete = 0;
int toAdd = 0;
int updateFlag = 0;
int verificationFlag = -1;
int networkFlag = 0;

uint32_t timer = 0;

int timestamp;


uint32_t updateStack = 0;
uint32_t relayStack = 0;
uint32_t ledStack = 0;
uint32_t heap = 0;

int fileOpenFlag = 0;

String fileTempPassword;
typedef struct html {
  char* content;
  uint32_t size;
} html;

typedef struct _localKeys {
  uint64_t key;
  int printPosition;
} localKeys;

localKeys* keylog = (localKeys*)malloc(sizeof(localKeys) * MAX_KEYS);
localKeys* addLog = (localKeys*)malloc(sizeof(localKeys) * MAX_UPDATE_KEYS);
localKeys* delLog = (localKeys*)malloc(sizeof(localKeys) * MAX_UPDATE_KEYS);

html index_html;

File upld;

bool Primary_Request_Done = false;
uint32_t repeatRequestTimer = 0;

static SemaphoreHandle_t mutex;

int id;

int UPDATE_ALLOWED = 1;
//stack* availableSlots = createStack();

//===============================================FUNCTION DECLARATION========================================================//

bool RequestKeys();
void swap(uint64_t* a, uint64_t* b);
int partition (uint64_t arr[], int low, int high);
void bubblesort(localKeys* a, int len);
uint64_t convertStringtoUint64(String dat);
void saveKeystoStorage();
uint32_t searchArgument(uint32_t itr, String argument, char* source, uint32_t filesize);
void updatePrefill();
void loadhtml();
int getEmptySlot();
void delete_FP_Slot(int slot);
int getFingerprintEnroll();
uint8_t deleteFingerprint(int id);
bool verifyPrint(int ID, uint64_t* tempKey);
