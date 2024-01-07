String getParams(String dat, AsyncWebServerRequest *request) {
  //Serial.printf("Looking for the value:%s,%sEND\n",dat,dat.c_str());
  int params = request->params();
  for (int i = 0; i < params; i++) {
    AsyncWebParameter* p = request->getParam(i);
    if (p->isFile()) {
      Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
    } else if (p->isPost()) {
      //Serial.printf("Comparing [%s] with [%s]\n",p->name().c_str(),dat.c_str());
      if (String(p->name().c_str()) == String(dat.c_str())) {
        //Serial.println("Compare Successful...");
        String returnVal = p->value().c_str();
        return returnVal;
      }
      Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    } else {
      Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
  }
  return "-1";
}


void setSSID(AsyncWebServerRequest *request) {
  ssid = getParams("ssid", request);
  password = getParams("pswd", request);
  loadhtml();
  request->send_P(200, "text/html", index_html.content);
  updateCredentials(ssid, password);
  Serial.println("------------------------------------------------------------------------------");
}

void setAPI(AsyncWebServerRequest *request) {
  unique_id = getParams("uuid", request);
  api_key = getParams("apiKey", request);
  loadhtml();
  request->send_P(200, "text/html", index_html.content);
  updateApi(api_key, unique_id);
  Serial.println("------------------------------------------------------------------------------");
  delay(10);
  RequestKeys();
}


void restartESP(AsyncWebServerRequest *request) {
  Serial.println("Restarting the device......");
  request->send_P(200, "text/html", restartPage);
  ESP.restart();
}


void updateHTML(AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
  tempUploadedFile = "/" + filename;
  String flnm = "/temp.bin";
  if (float(SPIFFS.totalBytes() - SPIFFS.usedBytes()) / (1024 ) >= 200) {
    if (!index) {
      Serial.println("------------------------------------------------------------------------------");
      Serial.printf("UploadStart: %s\n", filename.c_str());
    }
    if (fileOpenFlag == 0) {
      Serial.println("Trying to open the file:" + flnm);
      upld = SPIFFS.open(flnm, FILE_WRITE);
      if (upld) {
        Serial.println("File Opened Successfully. Writing the Data.");
      }
      fileOpenFlag = 1;
    }
    if (upld) {
      if (upld.write(data, len)) {
        Serial.print('.');
      }
      else {
        Serial.println("Not able to write the data in the file");
      }
    }
    else {
      Serial.println("Unable to open the File to do the Write Operation...");
    }
  }
  else {
    Serial.println("Running out of Storage.......");
  }
  if (final) {
    Serial.printf("UploadEnd: % s ( % u)\n", filename.c_str(), index + len);
    if (fileOpenFlag == 1) {
      upld.close();
      Serial.println("The file Closed Successfully...");
      fileOpenFlag = 0;
    }
    Serial.println("------------------------------------------------------------------------------");
  }
}

void resetDevice(AsyncWebServerRequest *request) {
  Serial.println("Reset In Process.........");
  if (SD.remove(filename)) {
    Serial.println("Deleted: " + String(filename));
  }
  else {
    Serial.println("Deletion Failed....");
  }

  ssid = "";
  password = "";
  updateCredentials(ssid, password);

  unique_id = "";
  api_key = "";
  updateApi(api_key, unique_id);

  loadhtml();
  request->send_P(200, "text / html", index_html.content);
}


void sendHomePage(AsyncWebServerRequest * request) {
  Serial.println("Sending the homepage to server....");
  //Serial.println(index_html.content);
  AsyncWebServerResponse *response = request->beginResponse(200,"text/html",index_html.content);
//  for(uint16_t i = 0; i < index_html.size; i++){
//    response->printf(index_html.content.c_str());
//  }
  request->send(response);
  //request->send_P(200, "text / html", index_html.content);
}
