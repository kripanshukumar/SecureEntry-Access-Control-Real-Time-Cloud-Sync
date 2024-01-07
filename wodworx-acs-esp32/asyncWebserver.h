void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  } else if (type == WS_EVT_ERROR) {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if (type == WS_EVT_PONG) {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len) {
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

      if (info->opcode == WS_TEXT) {
        for (size_t i = 0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for (size_t i = 0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if (info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0) {
        if (info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

      if (info->opcode == WS_TEXT) {
        for (size_t i = 0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for (size_t i = 0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if ((info->index + len) == info->len) {
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if (info->final) {
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
          if (info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

//===========================================================================================================================================

void initAsyncWebserver() {
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  delay(10);

  server.on("/submit", HTTP_POST, [](AsyncWebServerRequest * request) {
    setSSID(request);
  });
  server.on("/submit_api", HTTP_POST, [](AsyncWebServerRequest * request) {
    setAPI(request);
  });
  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
    restartESP(request);
  });
  //server.serveStatic("/", SPIFFS, "/source.html");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    sendHomePage(request);
  });

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    resetDevice(request);
  });


  server.on("/", HTTP_POST, [](AsyncWebServerRequest * request) {
    //Serial.println("In the first Section");
    if (float(SPIFFS.totalBytes() - SPIFFS.usedBytes()) / (1024 ) >= 200) {
      fileTempPassword = getParams("filepassword", request);
      Serial.println("The Password Received: " + fileTempPassword);

      if (fileTempPassword == String(filePassword)) {
        Serial.print("Checking for the existing file with the name: ");
        Serial.println(tempUploadedFile);
        if (SPIFFS.remove(tempUploadedFile)) {
          Serial.println("- file Deleted");
        } else {
          Serial.println("-" + String(tempUploadedFile) + " does not exist..");
        }

        delay(5);
        Serial.printf("Renaming file %s to %s\r\n", "/temp.bin", tempUploadedFile.c_str());
        if (SPIFFS.rename("/temp.bin", tempUploadedFile.c_str())) {
          Serial.println("- file renamed");
        } else {
          Serial.println("- rename failed");
        }
        loadhtml();
        delay(5);
        request->send_P(200, "text/html", index_html.content);
      }
      else {
        Serial.println("Wrong Password.....");
        request->send_P(200, "text/html", "Wrong Password");
      }
      Serial.println("\nDeleting the Temporary Downloaded File..");
      if (SPIFFS.remove("/temp.bin")) {
        Serial.println("- file Deleted");
      } else {
        Serial.println("-/temp.bin does not exist..");
      }
    }
    else {
      Serial.println("Nothing Update.....");
      request->send_P(200, "text-html", "Insufficient Storage.........");
    }

    Serial.println("=======================================================================================");
    Serial.print("Total Storage of SPIFFS:");
    Serial.println(float(SPIFFS.totalBytes()) / 1024);
    Serial.print("Available Space: ");
    Serial.println(float(SPIFFS.totalBytes() - SPIFFS.usedBytes()) / (1024 ));
  }, updateHTML);

  server.begin();
}
