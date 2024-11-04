#ifndef BLDC_UART_H
#define BLDC_UART_H

#include "BLDC_website.h"
#include "BLDC_driver.h"
#include "BLDC_EngineRPM.h"
#include <HardwareSerial.h>
#include "driver/uart.h"

#define BUFFER_SIZE 100


void initSerial();
void IRAM_ATTR onReceive();
void checkUarts();
void parseSCPICommand(char *scpiCommand);


#endif