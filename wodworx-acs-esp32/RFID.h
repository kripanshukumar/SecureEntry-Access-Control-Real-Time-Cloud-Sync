
void initRFID() {
  SPI.begin(18,19,23,5);                                                      // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
}

void restartRFID(){
  Serial.println("\nRestarting RFID Device.....");
  digitalWrite(RFID_RST_PIN,LOW);
  delay(10);
  digitalWrite(RFID_RST_PIN,HIGH);
  delay(10);
  mfrc522.PCD_Init();
  delay(100);
  Serial.println("Device is ready!!!");
  Serial.println("===================================================================================");
  
}

uint64_t readRFID() {
  uint64_t out = 0;
  String Name;
  String ID;
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return out;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return out;
  }

  Serial.println("===================================================================================");
  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card
  //Serial.print("UID is: ");
  uint64_t keyonCard = 0;
  int sz = mfrc522.uid.size;
  for(int l = 0; l < sz; l++){
    keyonCard += (uint64_t(mfrc522.uid.uidByte[sz - l-1]) * pow(16,2*l));
//    Serial.print(mfrc522.uid.uidByte[sz - l]);
//    Serial.print(" ");
//    Serial.print((uint64_t(mfrc522.uid.uidByte[sz - l-1]) * pow(16,2*l)));
//    Serial.print(" ");
//    Serial.println(keyonCard);
  }
  Serial.println(keyonCard);
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  //Serial.print(F("Name: "));

  byte buffer1[18];

  block = 4;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    restartRFID();
    return out;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    restartRFID();
    return out;
  }

  //PRINT FIRST NAME
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      Name.concat(char(buffer1[i]));
      //Serial.write(buffer1[i]);
    }
  }
  //Serial.print(" ");

  //---------------------------------------- GET LAST NAME

  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    restartRFID();
    return out;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    restartRFID();
    return out;
  }

  //PRINT LAST NAME
  for (uint8_t i = 0; i < 16; i++) {
    //Serial.write(buffer2[i] );
    ID.concat(char(buffer2[i]));
  }
  out = keyonCard;

  //----------------------------------------

  Serial.println(F("\n**End Reading**\n"));

  //delay(100); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  //Serial.println("Name: " + Name);
  //Serial.println("ID : " + ID);
  Serial.println("===================================================================================");
  return out;
}
