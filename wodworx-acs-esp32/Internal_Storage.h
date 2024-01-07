void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void initInternalStorage() {
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  Serial.print("Total Storage of SPIFFS:");
  Serial.println(String(float(SPIFFS.totalBytes()) / 1024) + "KB");
  Serial.print("Available Space: ");
  Serial.println(String(float(SPIFFS.totalBytes() - SPIFFS.usedBytes()) / 1024) + "KB");

  Serial.println("===========================Contents of Internal Storage================================");
  listDir(SPIFFS, "/", 0);
  Serial.println("=======================================================================================");
  File file = SPIFFS.open("/credentials.txt", FILE_READ);
  if (file) {
    Serial.println("File opened->Fetching Credentials..");
    ssid = file.readStringUntil(',');
    password = file.readStringUntil(',');
    Serial.println("SSID: " + ssid);
    Serial.println("Password: " + password);
    file.close();
  }
  else {
    Serial.println("Credentials.txt does not exist. Creating new credential file....");
    File file = SPIFFS.open("/credentials.txt", FILE_WRITE);
    if (file) {
      file.print(ssid);
      file.print(',');
      file.print(password);
      file.print(',');
      file.close();
    }
    else {
      Serial.println("Failed to create file..");
    }
  }

  File api_file = SPIFFS.open("/api.txt", FILE_READ);
  if (api_file) {
    Serial.println("File opened->Fetching api and unique id..");
    api_key = api_file.readStringUntil(',');
    unique_id = api_file.readStringUntil(',');
    Serial.println("api_key: " + api_key);
    Serial.println("unique_id: " + unique_id);
    api_file.close();
  }
  else {
    Serial.println("api.txt does not exist. Creating new API file....");
    File api_file = SPIFFS.open("/api.txt", FILE_WRITE);
    if (api_file) {
      api_file.print(api_key);
      api_file.print(',');
      api_file.print(unique_id);
      api_file.print(',');
      api_file.close();
    }
    else {
      Serial.println("Failed to create file..");
    }
  }
}

void updateCredentials(String id, String pass) {
  Serial.println("\nSSID and Password has been updated!!");
  File  file = SPIFFS.open("/credentials.txt", FILE_WRITE);
  if (file) {
    file.print(id);
    file.print(',');
    file.print(pass);
    file.print(',');
    file.close();
  }
  else {
    Serial.println("Failed to create file..");
  }

}

void updateApi(String key, String id) {
  Serial.println("\nAPI-Key and Unique-ID has been updated!!");
  File  file = SPIFFS.open("/api.txt", FILE_WRITE);
  if (file) {
    file.print(key);
    file.print(',');
    file.print(id);
    file.print(',');
    file.close();
  }
  else {
    Serial.println("Failed to create file..");
  }

}

uint32_t searchArgument(uint32_t itr, String argument, char* source, uint32_t filesize) {
  int itr2 = 0;
  uint32_t begning = 0;
  while (1) {
    //Serial.print(source[itr]);
    if (argument[0] == source[itr]) {
      begning = itr;
      while (argument[itr2++] == source[itr++]) {
        //Serial.print(source[itr]);
        if (itr2 == argument.length() - 1) {
          return begning;
        }
      }
      itr2 = 0;
    }
    itr++;
    if (itr >= filesize) {
      break;
    }
  }
  return -1;
}

void updatePrefill() {
  uint32_t pos = 0;
  pos = searchArgument(0, "ssid-holder", index_html.content, index_html.size);
  //Serial.println("Index of ssid-holder: " + String(pos));
  if (pos != 4294967295) {
    for (int i = 0; i < 20; i++) {
      if (i < ssid.length()) {
        index_html.content[pos + i] = ssid[i];
      }
      else {
        index_html.content[pos + i] = ' ';
      }
    }
  }
  else {
    pos = 0;
  }
  //...................................................................................
  pos = searchArgument(pos, "password-holder", index_html.content, index_html.size);
  //Serial.println("Index of password-holder: " + String(pos));
  if (pos != 4294967295) {
    for (int i = 0; i < 20; i++) {
      if (i < password.length()) {
        index_html.content[pos + i] = password[i];
      }
      else {
        index_html.content[pos + i] = ' ';
      }
    }
  }
  else {
    pos = 0;
  }
  //...................................................................................
  pos = searchArgument(pos, "uuid-holder", index_html.content, index_html.size);
  //Serial.println("Index of uuid-holder: " + String(pos));
  if (pos != 4294967295) {
    for (int i = 0; i < 32; i++) {
      if (i < unique_id.length()) {
        index_html.content[pos + i] = unique_id[i];
      }
      else {
        index_html.content[pos + i] = ' ';
      }
    }
  }
  else {
    pos = 0;
  }
  //...................................................................................
  pos = searchArgument(pos, "apikey-holder", index_html.content, index_html.size);
  //Serial.println("Index of apikey-holder: " + String(pos));
  if (pos != 4294967295) {
    for (int i = 0; i < 25; i++) {
      if (i < api_key.length()) {
        index_html.content[pos + i] = api_key[i];
      }
      else {
        index_html.content[pos + i] = ' ';
      }
    }
  }
  else {
    pos = 0;
  }
  //Serial.println(index_html.content);
}

void loadhtml() {
  Serial.println("------------------------------------------------------------------------------");
  Serial.println("\nLoading Index HTML in RAM.....");
  //  File file = SPIFFS.open("/source.html", FILE_READ);
  //  if (file) {
  //    if (file.size() <= 40000) {
  free(index_html.content);
  index_html.content = (char *)malloc(sizeof(source) + 1);
  index_html.size = sizeof(source);
  //      index_html.size = file.size();
  //      Serial.println("The size of File is: " + String(file.size()));
  //      file.read((uint8_t *)index_html.content, file.size());
  memcpy((void*)index_html.content, source, sizeof(source));
  Serial.println("The size of File is: " + String(sizeof(source)));
  index_html.content[index_html.size] = '\0';
  //Serial.println(index_html.content);
  updatePrefill();
  //Serial.println(index_html.content);
  //    }
  //    else {
  //      Serial.println("can't Load. File is Greater than 40000 bytes!!!!");
  //    }
  //  }
  //  else {
  //    Serial.println("Failed to load Index HTML!!!");
  //  }
  Serial.println("------------------------------------------------------------------------------");
}
