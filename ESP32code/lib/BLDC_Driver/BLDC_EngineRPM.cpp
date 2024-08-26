#include "BLDC_EngineRPM.h"

volatile int engineRPM = 0;    
volatile int pulseCount = 0;   


void IRAM_ATTR onPulse() {
    pulseCount++;
}


void setupEngineRPM() {
    pinMode(34, INPUT_PULLUP); // GPIO34 as input with pull-up
    attachInterrupt(digitalPinToInterrupt(34), onPulse, RISING); //count rising edges

    xTaskCreatePinnedToCore(
        engineRPMTask,         // Task function
        "EngineRPMTask",       // Task name
        4096,                  // Stack size (bytes)
        NULL,                  // Parameters
        1,                     // Priority
        NULL,                  // Task handle
        1                      // Core ID (1 for core 1, the main loop is core 0)
    );
}

void engineRPMTask(void *pvParameters) {
    const unsigned long interval = 1000; // 1 second interval
    unsigned long previousMillis = 0;

    while (true) {
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            // Update engineRPM with the count of pulses in the last second
            engineRPM = pulseCount;
            pulseCount = 0;
        }
    }
}