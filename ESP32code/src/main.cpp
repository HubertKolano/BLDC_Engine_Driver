#include "BLDC_driver.h"
#include "BLDC_EngineRPM.h"
#include "BLDC_website.h"
#include "BLDC_UART.h"
#include <Arduino.h>

void setup() {

  initAll();
  initSerial();
  setupEngineRPM();

  String ssid, password;
  readWiFiCredentials(ssid, password);
  initWIFI(ssid, password);

  serialMenuMessage();
}

void loop() {
  if(turnEngineControlPID && checkElapsedTime(DELTA_TIME_PID)){
    regulateRPMWithPID();
    OLEDrpmRead(engineReadRPM);
    }
  else if (checkElapsedTime(DELTA_TIME_PID) || checkElapsedTime(1000)){
    OLEDrpmRead(engineReadRPM);
  }
  checkButtons();
  checkUart();

  

}
