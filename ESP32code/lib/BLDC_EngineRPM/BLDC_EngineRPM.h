#ifndef BLDC_ENGINERPM_H
#define BLDC_ENGINERPM_H

#include <Arduino.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <esp_task_wdt.h>


#define INTERVAL_READOUT 1000
#define ENGINE_PPR 0.7

// Global variable to store readings of engine RPM
extern volatile int engineReadRPM;


void setupEngineRPM();
void engineRPMTask(void *pvParameters);

#endif