#include <BLDC_driver.h>
#include <BLDC_EngineRPM.h>
#include <Arduino.h>
static int voltage = 0;

void setup() {
    initOLED();
    
    initSerial();

    initButtons();

    setupTimer();

    setupEngineRPM();

    setCombinedDACOutput(voltage);
    welcomeOLED(voltage, 2000, false);
    // delay(5000);
    // voltage = 2500;
    // updateOLED(voltage, 5000, true);
    // setCombinedDACOutput(voltage);
    // myFunction();
  
}

void loop() {
    if(checkElapsedTime(2000)){
      static int i = 0;
      setCombinedDACOutput(i);
      i += 500;
      i = i <= 5000 ? i : 0 ;

    }
  Serial.print("Engine RPM: ");
  Serial.println(engineRPM);
  delay(1000);
  checkButtons();

}
