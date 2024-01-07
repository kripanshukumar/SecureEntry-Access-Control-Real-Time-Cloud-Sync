void initStorage() {
  pinMode(4, INPUT_PULLUP);
  spiSD.begin(14, 4, 13, SD_CS); //CLK,MISO,MOIS,SS
  if (!SD.begin(SD_CS, spiSD))
  {
    Serial.println("Card Mount Failed");
  }
  else
  {
    Serial.println("Card Mount Successful");
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  delay(100);
}

//===========================================================================================================================================//

void listDir(const char * dirname, uint8_t levels) {
  Serial.println("===========================Contents of Internal Storage================================");
  Serial.printf("Listing directory: %s\n", dirname);

  File root = SD.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.print (file.name());
      time_t t = file.getLastWrite();
      struct tm * tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, ( tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
      if (levels) {
        listDir(file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.print(file.size());
      time_t t = file.getLastWrite();
      struct tm * tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, ( tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
    }
    file = root.openNextFile();
  }
  file.close();
  Serial.println("=======================================================================================");
}

//===========================================================================================================================================//

void readFile(String Filename) {
  File file = SD.open(Filename, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for writing");
  }
  else {
    while (file.available()) {
      Serial.print(char(file.read()));
    }
    file.close();
  }
}

//===========================================================================================================================================//

void loadKeysFromStorage() {
  String temp;
  int iteration = 0;
  File file = SD.open(filename, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for Reading");
  }
  else {
    int index = 0;
    localKeys* temp = keylog;
    while (file.available()) {
      if (file.read((uint8_t *)temp, sizeof(localKeys))) {
        temp++;
        index++;
      }
      else {
        Serial.println("Read Failed!!!");
      }
      keysAvailable = index;
    }
    for (int i = 0; i < keysAvailable; i++) {
      //      Serial.print(i); Serial.print(". ");
      //      Serial.print(keylog[i].key); Serial.print(", "); Serial.println(keylog[i].printPosition);
    }
  }
  file.close();
}

//============================================================================================================================================//

void saveKeystoStorage() {
  File dataFile = SD.open(filename, FILE_WRITE);
  if (!dataFile) {
    Serial.println("Failed to open file for writing");
  }
  else {
    if (keysAvailable > 0) {
      for (int i = 0; i < keysAvailable; i++) {
        if (!dataFile.write((uint8_t*)&keylog[i], sizeof(localKeys))) {
          Serial.print(i); Serial.print('\t'); Serial.println("Write Failed!!!");
        }
        //        else{
        //          Serial.print(i);Serial.print('\t');Serial.print(keylog[i].key);Serial.print('\t');Serial.println(keylog[i].printPosition);
        //        }
      }
    }
  }
  dataFile.close();
}
