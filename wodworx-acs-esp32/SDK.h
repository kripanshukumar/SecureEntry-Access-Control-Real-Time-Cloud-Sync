//=====================================================================================================================================================//
void bubblesort(localKeys* a, int len) {
  uint64_t key;
  int fingerID;
  for (int i = 0; i < len - 1; i++) {
    for (int j = 0; j < len - i - 1; j++) {
      if (a[j].key > a[j + 1].key) {
        key = a[j + 1].key;
        fingerID = a[j + 1].printPosition;
        a[j + 1].key = a[j].key;
        a[j + 1].printPosition = a[j].printPosition;
        a[j].key = key;
        a[j].printPosition = fingerID;
      }
    }
  }
}

int getEmptySlot() {
  for (int i = 1; i < NO_OF_FP_SLOTS + 1; i++) {
    if (takenFingerPos[i] == 0) {
      return i;
    }
  }
  Serial.println("No Empty slot found....");
  return -1;
}

void delete_FP_Slot(int slot) {
  if (slot > 0 && slot < NO_OF_FP_SLOTS + 1) {
    takenFingerPos[slot] = 0;
  }
  else {
    Serial.println("Index out of range...");
  }
}

//====================================================================================================================================

void enrol_on_button_push(){
  uint64_t KeyOnRFID = 0;
  if (digitalRead(ENROLL_BUTTON) == 0) {
    Serial.println("\n--------------------------------------------------------------------------------");
    Serial.println("Finger print Enrolment started: ");
    int slot = getEmptySlot();
    if (slot > 0) {
      Serial.print("The Empty Slot found for registration is: "); Serial.println(slot);
      id = slot;
      Serial.print("Enrolling ID #");
      Serial.println(id);
      digitalWrite(ORANGE_LED, HIGH);
      while (!  getFingerprintEnroll() ) {
        delay(10);
      }
      uint32_t localTimer = 0;
      int itr = 0;
      while (KeyOnRFID == 0) {
        KeyOnRFID = readRFID();
        if (millis() - localTimer >= 500) {
          digitalWrite(ORANGE_LED, !digitalRead(ORANGE_LED));
          localTimer = millis();
          itr++;
          Serial.print("ITR: ");Serial.println(itr);
          if (itr >= 20) {
            deleteFingerprint(id);
            break;
          }
        }
      }
      if (itr < 20) {
        UPDATE_ALLOWED = 0;
        attach_print_to_key(id, KeyOnRFID);
        UPDATE_ALLOWED = 1;
      }
      delay(500);
      digitalWrite(ORANGE_LED, LOW);
    }
    else {
      Serial.println("No empty slot found");
    }
  }
}

//=====================================================================================================================================================//
void activateRelay(void *parameter) {
  while (1) {
    if (verificationFlag == 1) {
      digitalWrite(RelayPin, HIGH);
      delay(RELAY_DELAY);
      digitalWrite(RelayPin, LOW);
    }
    relayStack = uxTaskGetStackHighWaterMark(NULL);
    delay(100);
  }
}
bool verifyPrint(int ID, uint64_t* tempKey) {
  bool access;
  Serial.println("===================================================================================");
  Serial.println("Authenticating.................\n");
  int matchFound = 0;
  for (int m = 0; m < keysAvailable; m++) {
    if (ID == keylog[m].printPosition) {
      matchFound = m + 1;
      break;
    }
  }
  if (matchFound == 0) {
    verificationFlag = 0;
    Serial.println("Invalid ID, please contact the administrator...");
    access = false;
  }
  else {
    verificationFlag = 1;
    *tempKey = keylog[matchFound - 1].key;
    Serial.print("Match found for Key : ");
    Serial.println(keylog[matchFound - 1].key);
    access = true;
  }
  return access;
}

bool verifyKey(uint64_t RFID_KEY) {
  bool access;
  Serial.println("===================================================================================");
  Serial.println("Authenticating.................\n");
  int matchFound = 0;
  for (int m = 0; m < keysAvailable; m++) {
    if (RFID_KEY == keylog[m].key) {
      matchFound = m + 1;
      break;
    }
  }
  if (matchFound == 0) {
    verificationFlag = 0;
    Serial.println("Invalid Key, please check you card or contact the administrator...");
    access = false;
  }
  else {
    //    if (matchFound > 1) {
    //      Serial.println("More than one Entry found..");
    //    }
    //    else {
    verificationFlag = 1;
    Serial.print("Match found for Key : ");
    Serial.println(RFID_KEY);
//    if (FingerPrintEnabled == 1) {
//      if (keylog[matchFound - 1].printPosition == 0) {
//        Serial.println("No Finger Print is attached with this key....");
//        int slot = getEmptySlot();
//        if (slot > 0) {
//          Serial.print("The Empty Slot found for registration is: "); Serial.println(slot);
//          id = slot;
//          Serial.print("Enrolling ID #");
//          Serial.println(id);
//          uint32_t localTimer = 0;
//          UPDATE_ALLOWED = 0;
//          while (digitalRead(ENROLL_BUTTON) == 1) {
//            delay(100);
//            //Serial.print('.');
//            if (millis() - localTimer >= 500) {
//              digitalWrite(ORANGE_LED, !digitalRead(ORANGE_LED));
//              localTimer == millis();
//            }
//          }
//          digitalWrite(ORANGE_LED, HIGH);
//          while (!  getFingerprintEnroll() ) {
//            delay(10);
//          }
//          attach_print_to_key(id, RFID_KEY);
//          digitalWrite(ORANGE_LED, LOW);
//        }
//      }
//    }
    UPDATE_ALLOWED = 1;
    access = true;
    if (keylog[matchFound - 1].key <= 0) {

      //getFingerprintEnroll();
    }
  }
  Serial.println("===================================================================================");
  return access;
}

//=====================================================================================================================================================//

int checkKey(localKeys* localArray, int localItr, localKeys* delArray, int delItr) {
  if (localItr > keysAvailable || delItr >= toDelete) {
    return 0;
  }
  if (localArray[localItr].key == delArray[delItr].key) {
    return (1 + checkKey(localArray, localItr + 1, delArray, delItr + 1));
  }
  else {
    return 0;
  }
}

int deleteElements(localKeys* arr, int arraylength, localKeys* deleteionArray, int deletionLenght) {
  int iteration = 0;
  int delIteration = 0;
  int skipLength = 0;
  int difference = 0;
  for (iteration = 0; iteration < arraylength; iteration++) {
    if (delIteration < deletionLenght) {
      skipLength = checkKey(arr, iteration, deleteionArray, delIteration);
      Serial.println("Skip Length = " + String(skipLength));
    }
    iteration += skipLength;
    delIteration += skipLength;
    difference += skipLength;
    arr[iteration - difference].key = arr[iteration].key;
    arr[iteration - difference].printPosition = arr[iteration].printPosition;
    skipLength = 0;
    if (delIteration >= deletionLenght) {
      arr[iteration].key = 0;
      if (FingerPrintEnabled == 1) {
        deleteFingerprint(arr[iteration].printPosition);
      }
      arr[iteration].printPosition = 0;
    }
  }

  return (iteration - difference - 1);

}


int addElements(localKeys* arr, int arrayLength, localKeys* addArray, int additionLength) {
  if (additionLength + keysAvailable > MAX_KEYS) {
    Serial.println("The addition list length is more than space available.........");
    Serial.println("Adding the keys which can be stored in existing space.........");
    //memcpy(&arr[keysAvailable - 1], addArray, sizeof(localKeys) * (MAX_KEYS - keysAvailable));
    for (int i = keysAvailable; i < MAX_KEYS; i++) {
      arr[i].key = addArray[i - keysAvailable].key;
      arr[i].printPosition = addArray[i - keysAvailable].printPosition;
    }
    bubblesort(arr, MAX_KEYS);
    return MAX_KEYS;
  }
  else {
    //memcpy(&arr[keysAvailable - 1], addArray, sizeof(localKeys) * (additionLength));
    for (int i = keysAvailable; i < arrayLength + additionLength; i++) {
      arr[i].key = addArray[i - keysAvailable].key;
      arr[i].printPosition = addArray[i - keysAvailable].printPosition;
    }
    bubblesort(arr, arrayLength + additionLength);
    return arrayLength + additionLength;
  }
}


void updateKeys(void *parameter) {
  while (1) {
    if (WiFi.status() == WL_CONNECTED && UPDATE_ALLOWED == 1) {
      networkFlag = 3;
      //Serial.print("\nLooking for new keys->");
      if (Primary_Request_Done == true) {
        if (xSemaphoreTake(mutex, 0) == pdTRUE) {
          if (canUpdate()) {
            Serial.print("Update Available->");
            getChanges();
            Serial.print("Getting Changes->");
            updateFlag = 1;

            if (toDelete > 0) {
              Serial.print("Deleting the Keys->");
              keysAvailable = deleteElements(keylog, keysAvailable, delLog, toDelete);
            }
            if (toAdd > 0) {
              Serial.print("Adding Keyss->");
              keysAvailable = addElements(keylog, keysAvailable, addLog, toAdd);
            }

            updateFlag = 0;
            Serial.println("Saving keys to Local Storage..");
            delay(5);
            saveKeystoStorage();
            //            for (int i = 0; i < keysAvailable; i++) {
            //              Serial.print(i); Serial.print('\t');
            //              Serial.println(keylog[i].key);
            //            }
          }
          xSemaphoreGive(mutex);
          delay(10);
        }
        else {
          delay(UPDATE_INTERVAL);
          //Serial.println("Nothing to update....");
        }
      }
      networkFlag = 1;
    }
    else {
      networkFlag = 0;
    }
    updateStack = uxTaskGetStackHighWaterMark(NULL);

    delay(UPDATE_INTERVAL);
  }
}

//=====================================================================================================================================================//

uint64_t convertStringtoUint64(String dat) {
  uint64_t intValue = 0;
  int lnt = dat.length();
  int temp = 0;
  int increment = 0;
  //Serial.print("The length of string is: ");Serial.print(lnt);Serial.print('\t');
  //Serial.println(dat);
  for (int i = 0; i < lnt; i++) {
    temp = int(dat[lnt - i - 1]);
    if ((temp > 47) && (temp <= 57)) {
      intValue += ((dat[lnt - i - 1] - 48) * pow(10, increment));
      //Serial.print(dat[lnt - i - 1]); Serial.print(' '); Serial.println(intValue);
      increment++;
    }
  }
  return intValue;
}

//=====================================================================================================================================================//

void reconnectThread(void* parameter) {
  while (1) {
    delay(1000);
    //Serial.println(WiFi.status());
    if (WiFi.status() != 3) {
      Primary_Request_Done = false;
      WiFi.disconnect();
      delay(1000);
      Serial.print("Connecting to ");
      Serial.println(ssid);
      WiFi.begin(ssid.c_str(), password.c_str());
      int iteration = 0;
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        iteration++;
        if (iteration >= 16) {
          break;
        }
      }
      if (WiFi.status() == WL_CONNECTED) {
        networkFlag = 1;
        Serial.println("Connected!");
        Serial.print ( "IP address: " );
        Serial.println ( WiFi.localIP() );
        RequestKeys();
      }
    }
  }
}

//=====================================================================================================================================================//
