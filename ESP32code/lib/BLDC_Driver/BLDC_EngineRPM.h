#ifndef ENGINE_RPM_H
#define ENGINE_RPM_H

#include <Arduino.h>

// Global variable to store readings of engine RPM
extern volatile int engineRPM;


void setupEngineRPM();
void engineRPMTask(void *pvParameters);

#endif