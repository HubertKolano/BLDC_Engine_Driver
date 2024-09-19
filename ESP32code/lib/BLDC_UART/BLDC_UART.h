#ifndef BLDC_UART_H
#define BLDC_UART_H

#include "BLDC_website.h"
#include "BLDC_driver.h"
#include "BLDC_EngineRPM.h"
#include <HardwareSerial.h>
#include "driver/uart.h"

#define BUFFER_SIZE 100


void initSerial();
void sendToBothUarts(String message);
void serial0MenuMessage();
String readFromUart0();
void checkUart();
void handleChoice1();
void handleChoice2();
void handleChoice3();
void handleChoice4();
void handleChoice5();
void handleChoice6();
void handleChoice7();
void handleChoice8();



void IRAM_ATTR onReceive();


void checkUarts();


void parseSCPICommand(char *scpiCommand);
#endif