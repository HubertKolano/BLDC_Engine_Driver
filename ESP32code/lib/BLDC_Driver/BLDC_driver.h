#ifndef BLDC_DRIVER_H
#define BLDC_DRIVER_H

#include <Arduino.h>
#include <BLDC_driver.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_GFX.h>


#define DAC1_PIN 25
#define DAC2_PIN 26 

//buttons names and timings
#define BUTTON1_PIN 18
#define BUTTON2_PIN 23
#define BUTTON3_PIN 19
#define BUTTON4_PIN 5

#define DEBOUNCE_TIME 50          
#define LONG_PRESS_TIME 1000000   
#define HOLD_PRESS_INTERVAL 100000 



// Function prototypes:

void initOLED();

void initSerial();

void initButtons();

void initTimer();

void updateOLED(int voltage, int rpm, bool direction);

void setCombinedDACOutput(int inputValue);


#endif