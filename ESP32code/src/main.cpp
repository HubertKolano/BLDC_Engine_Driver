#include <BLDC_driver.h>
#include <Arduino.h>
static int voltage = 5000;

void setup() {
  initOLED();
  
  initSerial();

  initButtons();

  //void initTimer();

  setCombinedDACOutput(voltage);
  updateOLED(voltage, 2000, false);
  // delay(5000);
  // voltage = 2500;
  // updateOLED(voltage, 5000, true);
  // setCombinedDACOutput(voltage);
  // myFunction();
}

void loop() {
 // Serial.println("Hello from the loop");
 // delay(1000);
 checkButtons();

}
