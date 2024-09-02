#include "BLDC_EngineRPM.h"
#include <esp_task_wdt.h>

volatile int engineReadRPM = 0;    
volatile int pulseCount = 0;   

TaskHandle_t Task1;

void IRAM_ATTR onPulse() {
    pulseCount++;
}

void setupEngineRPM() {
    pinMode(34, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(34), onPulse, RISING); 

    xTaskCreatePinnedToCore(
        engineRPMTask,         // Task function
        "EngineRPMTask",       // Task name
        4096,                  // Stack size (bytes)
        NULL,                  // Parameters
        1,                     // Priority
        &Task1,                // Task handle
        0                      // Core ID (Arduino main loop runs on core 1)
    );
}

void engineRPMTask(void *pvParameters) {
    unsigned long previousMillis = 0;

    while (true) {
        unsigned long currentMillis = millis();
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;

        if (currentMillis - previousMillis >= INTERVAL_READOUT) {
            previousMillis = currentMillis;
            // Update engineRPM with the count of pulses in the last second taking engine PPR into consideration
            engineReadRPM = pulseCount / ENGINE_PPR;
            pulseCount = 0;
        }
    }
}