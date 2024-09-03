#include "BLDC_driver.h"
#include "BLDC_EngineRPM.h"
#include <Arduino.h>

void setup() {

  initAll();

  setupEngineRPM();

  voltageDACS = 0;
  engineSetRPM = 0;
  engineDirection = ENGINE_CLOCKWISE;

  welcomeOLED(voltageDACS, engineSetRPM, engineDirection, engineReadRPM);
  // delay(5000);
  // voltage = 2500;
  // updateOLED(voltage, 5000, true);
  // setCombinedDACOutput(voltage);
  // myFunction();
}

void loop() {
  if(turnEngineControlPID && checkElapsedTime(DELTA_TIME_PID)){
    regulateRPMWithPID();
    OLEDrpmRead(engineReadRPM);
    }
  if(checkElapsedTime(3100)){
    OLEDrpmRead(engineReadRPM);
  }
  checkButtons();

}
