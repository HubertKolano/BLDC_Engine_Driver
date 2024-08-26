#ifndef BLDC_ENGINERPM_H
#define BLDC_ENGINERPM_H

#include <Arduino.h>

// Global variable to store readings of engine RPM
extern volatile int engineRPM;


void setupEngineRPM();
void engineRPMTask(void *pvParameters);

#endif