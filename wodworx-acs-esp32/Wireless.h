//=====================================================================================================================================================//
void initWiFi() {
  WiFi.disconnect();
  delay(1000);

  //WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  WiFi.softAP(espSSID.c_str(), espPassword.c_str());     //The IP address of ESP on Access Point network is  192.168.4.1

  Serial.println("===================================================================================");
  Serial.print("Connecting...");
  int itr = 0;
  networkFlag = 2;
  while (WiFi.status() != WL_CONNECTED)
  {
    itr++;
    Serial.print("..");
    delay(1000);
    if (itr >= 8) {
      if ( RESTART_ENABLE == 1) {
        Serial.println("\nToo long waiting for the connection. Restarting the ESP32....");
        ESP.restart();
      }
      else {
        break;
      }
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    networkFlag = 1;
    Serial.println("..");
    Serial.println("Connected!");
    Serial.print ( "IP address: " );
    Serial.println ( WiFi.localIP() );
    Serial.println("===================================================================================");
    //    if (MDNS.begin(host)) {
    //      MDNS.addService("http", "tcp", 80);
    //      Serial.println("MDNS responder started");
    //      Serial.print("Please connect to http://");
    //      Serial.print(host);
    //      Serial.println(".local");
    //      Serial.println("===================================================================================");
    //    }
  }
  else {
    Serial.println("\nUnable to connect to the WIFI");
    networkFlag = 0;
  }
  //    server.on("/submit", HTTP_POST, setSSID);
  //    server.on("/submit_api", HTTP_POST, setAPI);
  //    server.on("/restart", HTTP_GET, restartESP);
  //    //server.serveStatic("/", SPIFFS, "/source.html");
  //    server.on("/",HTTP_GET,sendHomePage);
  //    server.on("/",HTTP_POST,updateHTML);
  //    authenticateLogin();
  //    server.begin();
  initAsyncWebserver();
}

//=====================================================================================================================================================//

bool RequestKeys() {
  networkFlag = 3;
  if (WiFi.status() == WL_CONNECTED) {
    while (1) {
      if (xSemaphoreTake(mutex, 0) == pdTRUE) {
        break;
      }
      delay(3);
    }
    Serial.println("\nRequesting the file from the server.......");
    HTTPClient http; //Object of class HTTPClient

    // Begin request
    http.begin(listAccessKeys);
    http.addHeader("HTTP_X_ORG_API_KEY", api_key);
    http.addHeader("HTTP_X_DEVICE_ID", unique_id);

    int httpCode = http.GET();

    Serial.println("httpCode: " + String(httpCode));

    if (httpCode > 0)
    {
      // Get the respose as text

      String response = http.getString();
      const char* payload = response.c_str();//(char*)malloc((sizeof(char) * response.length()) + 1);
      //memcpy(payload, response.c_str(), response.length());
      //payload[response.length()] = '\0';
      //Serial.println(payload);

      Serial.println("-----------------------------------");
      Serial.println(String(response.length()) + " Characters loaded");
      Serial.println("-----------------------------------");

      // Prepare JSON parser
      const size_t bufferSize = response.length() * 2;
      Serial.println("Buffer size required: " + String(bufferSize));
      DynamicJsonBuffer jsonBuffer(bufferSize);
      // Parse JSON string
      JsonObject& root = jsonBuffer.parseObject(payload);
      if (!root.success()) {
        Serial.println("parseObject() failed");
      }
      else {
        if (root["statusCode"] == 200) {

          Serial.println("Status 200 :)");

          // Get the "keys" array
          JsonArray& data = root["data"];
          JsonArray& keys = root["data"]["keys"];
          timestamp = root["data"]["created_at"];

          // Get timestamp and save it
          Serial.println("timestamp: " + (String)timestamp);
          Serial.println("-----------------------------------");
          Serial.println("total Keys:" + String(keys.size()));
          Serial.println("-----------------------------------");
          int bound = keys.size();
          if (bound >= MAX_KEYS) {
            bound = MAX_KEYS;
          }
          for (int i = 0; i < bound; ++i) {
            String tmp = keys[i];
            String key = keys[i]["rf"];
            int po = keys[i]["po"];
            //Serial.println(tmp);
            if (FingerPrintEnabled == 1) {
              keylog[i].key = convertStringtoUint64(key);
              keylog[i].printPosition = po;
              if (po < 1000 && po > 0) {
                takenFingerPos[po] = 1;
              }
            }
            else {
              keylog[i].key = convertStringtoUint64(tmp);
              keylog[i].printPosition = 0;
            }
            //Serial.print(i); Serial.print('\t'); Serial.print(keylog[i].key); Serial.print('\t'); Serial.println(keylog[i].printPosition);
          }
          keysAvailable = bound;
          bubblesort(keylog, keysAvailable);
          delay(2);
          saveKeystoStorage();
          Primary_Request_Done = true;
        }
        else {
          Serial.println("CODE " + String(httpCode) + " :(");
        }
      }
      //free(payload);
    }

    http.end();
    Serial.println("File Closed...");
    Serial.println("===================================================================================");
    xSemaphoreGive(mutex);
    networkFlag = 1;
    return true;
  }
  networkFlag = 1;
  return false;
}

//=====================================================================================================================================================//

bool canUpdate() {
  //ledOn();
  //Serial.println("Can update?");
  //Serial.println("timestamp: " + (String)timestamp);

  HTTPClient http; //Object of class HTTPClient
  //  Serial.print("Connecting to ");
  //  Serial.println(checkChanges + (String)timestamp);
  // Begin request
  http.begin(checkChanges + (String)timestamp);
  http.addHeader("HTTP_X_ORG_API_KEY", api_key);
  http.addHeader("HTTP_X_DEVICE_ID", unique_id);

  int httpCode = http.GET();
  bool can = false;

  //Serial.println("httpCode: " + String(httpCode));

  if (httpCode > 0)
  {
    // Prepare JSON parser
    String response = http.getString();
    const char* payload = response.c_str();
    const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(1000);
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(payload);

    if (root["statusCode"] == 200) {
      //            Serial.println("Status 200 :)");
      //            Serial.println("-----------------------------------");
      //            Serial.println(String(response.length()) + " Characters loaded");
      //            Serial.println("-----------------------------------");
      //            Serial.println(response);
      //            Serial.println("-----------------------------------");

      // Get the "keys" array
      can = root["data"];
    }
  }

  http.end();
  delay(10);
  //ledOff();
  return can;
}

//=====================================================================================================================================================//

void getChanges() {
  //ledOn();
  HTTPClient http; //Object of class HTTPClient

  // Begin request
  http.begin(listKeyChanges + (String)timestamp);
  http.addHeader("HTTP_X_ORG_API_KEY", api_key);
  http.addHeader("HTTP_X_DEVICE_ID", unique_id);

  int httpCode = http.GET();

  Serial.println("httpCode: " + String(httpCode));

  if (httpCode > 0)
  {
    // Prepare JSON parser
    String response = http.getString();
    const char* payload = response.c_str();
    //char* payload = (char*)malloc((sizeof(char) * response.length()) + 1);
    //memcpy(payload, response.c_str(), response.length());
    const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(1000);
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(payload);

    if (root["statusCode"] == 200) {

      Serial.println("Status 200 :)");

      // Get the "keys" array
      JsonArray& data = root["data"];
      JsonArray& add = root["data"]["add"];
      JsonArray& del = root["data"]["delete"];
      timestamp = root["data"]["created_at"];

      // Get timestamp and save it
      //const char* created_at = data[0]["created_at"];
      Serial.println("timestamp: ");
      Serial.println(timestamp);

      toAdd = add.size();
      toDelete = del.size();
      if (toAdd >= MAX_UPDATE_KEYS) {
        toAdd = MAX_UPDATE_KEYS;
      }
      int delIndex = 0;
      for (int i = 0; i < toAdd; ++i) {
        addLog[i].key = convertStringtoUint64(add[i]["rf"]);
        //Serial.print("Addition buffer: ");Serial.println(addLog[i].key);
        addLog[i].printPosition = add[i]["po"];
      }

      bubblesort(addLog, toAdd);

      //Serial.printf("\n%d\n", delLog);

      for (int i = 0; i < toDelete; ++i) {
        delLog[i].key = convertStringtoUint64(del[i]["rf"]);
      }

      bubblesort(delLog, toDelete);

      //Serial.printf("\n%d\n", delLog);

      Serial.println("-----------------------------------");
      Serial.println(String(response.length()) + " Characters loaded");
      Serial.println("-----------------------------------");
      Serial.println(response);
      Serial.println("-----------------------------------");
      Serial.println("Add Keys:" + String(toAdd));
      Serial.println("Delete Keys:" + String(toDelete));
      Serial.println("-----------------------------------");
    }
    //free(payload);
  }

  delay(10);
  http.end();
}

void eventsToServer(uint64_t key, bool access) {

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    while (1) {
      if (xSemaphoreTake(mutex, 0) == pdTRUE) {
        break;
      }
      delay(7);
    }
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(accessEvent);

    // Specify content-type header
    http.addHeader("HTTP_X_ORG_API_KEY", api_key);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("HTTP_X_DEVICE_ID", unique_id);

    unsigned long long1 = (unsigned long)((key & 0xFFFFFFFF00000000) >> 32 );
    unsigned long long2 = (unsigned long)((key & 0xFFFFFFFF));
    String stringKey;
    if (long1 != 0) {
      stringKey = String(long1) + String(long2);
    }
    else {
      stringKey = String(long2);
    }
    String event;
    if (access == true) {
      event =  "\"access-allowed\"";
    }
    else {
      event = "\"access-denied\"";
    }
    // Data to send with HTTP POST
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["accessKeySystem"] = "wodworx";
    root["accessKeyID"] = stringKey;
    root["eventType"] = event;
    String postData;
    root.printTo(postData);
    // start connection and send HTTP header
    int httpCode = http.POST(postData);

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    // Free resources
    xSemaphoreGive(mutex);
  }
}

void attach_print_to_key(int fingerPos, uint64_t key) {
  if (WiFi.status() == WL_CONNECTED) {
    while (1) {
      if (xSemaphoreTake(mutex, 0) == pdTRUE) {
        break;
      }
      delay(5);
    }
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(sendFingerprint);

    // Specify content-type header
    http.addHeader("HTTP_X_ORG_API_KEY", api_key);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("HTTP_X_DEVICE_ID", unique_id);

    unsigned long long1 = (unsigned long)((key & 0xFFFFFFFF00000000) >> 32 );
    unsigned long long2 = (unsigned long)((key & 0xFFFFFFFF));
    String stringKey;
    if (long1 != 0) {
      stringKey = String(long1) + String(long2);
    }
    else {
      stringKey = String(long2);
    }

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["templatePos"] = String(fingerPos);
    root["accessKeyID"] = String(stringKey);
    String postData ;//= "templatePos:2\naccessKeyID:2580411092";
    root.printTo(postData);
    Serial.println(postData);

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header
    int httpCode = http.POST(postData);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    xSemaphoreGive(mutex);
  }

}
