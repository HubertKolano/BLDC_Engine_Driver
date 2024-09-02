#include "BLDC_driver.h"
#include "BLDC_EngineRPM.h"
#include <Arduino.h>

void setup() {
  initOLED();
  
  initSerial();

  initButtons();

  setupTimer();

  setupEngineRPM();
  voltageDACS = 2000;
  setCombinedDACOutput(voltageDACS);
  welcomeOLED(voltageDACS, 0, false, engineReadRPM);
  // delay(5000);
  // voltage = 2500;
  // updateOLED(voltage, 5000, true);
  // setCombinedDACOutput(voltage);
  // myFunction();
  
}

void loop() {
  if(checkElapsedTime(1000)){
    Serial.print("Engine RPM: ");
    Serial.println(engineReadRPM);
    OLEDrpmRead(engineReadRPM);
    }

  checkButtons();

}
