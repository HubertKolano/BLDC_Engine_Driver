#include <BLDC_driver.h>

static int voltage = 5000;

void setup() {
  initOLED();
  
  initSerial();
  
  setCombinedDACOutput(voltage);
  updateOLED(voltage, 2000, false);
  delay(5000);
  voltage = 2500;
  updateOLED(voltage, 5000, true);
  setCombinedDACOutput(voltage);
}

void loop() {
 // Serial.println("Hello from the loop");
 // delay(1000);


}
