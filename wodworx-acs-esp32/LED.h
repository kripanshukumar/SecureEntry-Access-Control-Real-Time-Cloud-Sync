void LEDS_BUZZERS(void *parameter) {
  int loopTime = 50;
  uint32_t t = 0;
  uint32_t green_t = 0;
  uint32_t red_t = 0;
  uint32_t buzzer_t = 0;
  uint32_t blue_t = 0;
  uint32_t red_itr = 6;
  uint32_t buzzer_itr = 2;
  uint32_t green_itr = GREEN_LED_ON_TIME / loopTime;
  uint32_t blue_itr = 0;
  while (1) {
    if (verificationFlag == 1) {
      if (millis() - green_t < GREEN_LED_ON_TIME) {
        digitalWrite(ACCESS_LED_RED, LOW);
        digitalWrite(ACCESS_LED_GREEN, HIGH);
      }
      else {
        digitalWrite(ACCESS_LED_RED, LOW);
        digitalWrite(ACCESS_LED_GREEN, LOW);
        verificationFlag = -1;
      }
      if (millis() - buzzer_t < 500) {
        //Serial.println("Buzzer Is HIGH");
        digitalWrite(BUZZER, HIGH);
      }
      else {
        digitalWrite(BUZZER, LOW);
      }
    }
    else {
      if (verificationFlag == 0) {
        if (red_itr > 0) {
          if (millis() - red_t > RED_LED_ON_TIME) {
            digitalWrite(ACCESS_LED_GREEN, LOW);
            digitalWrite(ACCESS_LED_RED, !digitalRead(ACCESS_LED_RED));
            red_t = millis();
            red_itr -= 1;
          }
        }
        else {
          digitalWrite(ACCESS_LED_RED, LOW);
          digitalWrite(ACCESS_LED_GREEN, LOW);
          red_itr = 6;
          verificationFlag = -1;
        }
        if (buzzer_itr > 0) {
          if (millis() - buzzer_t > BEEP_DURATION) {
            digitalWrite(BUZZER, !digitalRead(BUZZER));
            buzzer_t = millis();
            buzzer_itr -= 1;
          }
        }
        else {
          digitalWrite(BUZZER, LOW);
          buzzer_itr = 2;
        }
      }
      else {
        digitalWrite(BUZZER, LOW);
        buzzer_itr = 2;
        green_t = millis();
        red_t = millis();
        buzzer_t = millis();
      }
    }
    switch (networkFlag) {
      case 0:
        digitalWrite(NETWORK_LED, LOW);
        break;
      case 1:
        digitalWrite(NETWORK_LED, HIGH);
        break;
      case 2:
        if (millis() - blue_t > 1000) {
          digitalWrite(NETWORK_LED, !digitalRead(NETWORK_LED));
          blue_t = millis();
        }
        break;
      case 3:
        if (millis() - blue_t > 50) {
          digitalWrite(NETWORK_LED, !digitalRead(NETWORK_LED));
          blue_t = millis();
        }
        break;
    }
//    Serial.println("High water mark of \"LED\" (words): ");
//    Serial.println(uxTaskGetStackHighWaterMark(NULL));
  ledStack = uxTaskGetStackHighWaterMark(NULL);
//    Serial.println("Heap of \"LED\" (bytes): ");
//    Serial.println(xPortGetFreeHeapSize());
    delay(50);
  }
}

void initLEDS() {
  pinMode(ACCESS_LED_RED, OUTPUT);
  pinMode(ACCESS_LED_GREEN, OUTPUT);
  pinMode(NETWORK_LED, OUTPUT);
  pinMode(ORANGE_LED, OUTPUT);
  pinMode(ENROLL_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  digitalWrite(ACCESS_LED_RED , LOW);
  digitalWrite(ACCESS_LED_GREEN , LOW);
  digitalWrite(NETWORK_LED , LOW);
  digitalWrite(BUZZER, LOW);
  digitalWrite(RelayPin, LOW);
}
