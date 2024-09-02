#ifndef BLDC_DRIVER_H
#define BLDC_DRIVER_H

#include "BLDC_EngineRPM.h"
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

//UART defines
#define BAUD_RATE 115200

// oled defines
#define LINE1 14
#define LINE2 30
#define LINE3 46
#define LINE4 62
#define COLUMN1 10
#define COLUMN2 60
#define COLUMN3 100 

// DAC pins and values
#define DAC1_PIN 25
#define DAC2_PIN 26 
#define DAC_MAX_VOLTAGE 5000
#define DAC_HALF_MAX_VOLTAGE 2500
#define DAC_START_ENGINE_VOLTAGE 900

//RPM regulate function
#define PID_Kp 2.0
#define PID_Ki 0.1
#define PID_Kd 0.5
#define DELTA_TIME 3000

//buttons pins and timings
#define BUTTON1_PIN 18
#define BUTTON2_PIN 23
#define BUTTON3_PIN 19
#define BUTTON4_PIN 5
#define DEBOUNCE_TIME 50          
#define LONG_PRESS_TIME 1000
#define HOLD_PRESS_INTERVAL 100

#define ENGINE_CLOCKWISE false
#define ENGINE_COUNTERCLOCKWISE true
#define ENGINE_MAX_RPM 3000

extern int voltageDACS;
extern int engineSetRPM;
extern bool engineDirection;


//interface declaration
extern bool turnEngineControlPID;

struct Button {
    uint8_t pin;                    // Button GPIO pin
    void (*PressCallback)();    // Function pointer for short press action
    bool isPressed;                  // Button press status
    bool longPressTriggered;         // Has the long press been triggered?
    unsigned long lastPressTime;     // Last time the button was pressed
    unsigned long pressStartTime;    // When the button was first pressed
    unsigned long lastLongPressActionTime; // Last time the long press action was performed
};
// Function prototypes:

void initOLED();

void initSerial();

void initButtons();
void checkButtons();
void handleButtonPress(Button &button);
void button1Press();
void button2Press();
void button3Press();
void button4Press();

void setupTimer();
bool checkElapsedTime(unsigned long interval);
void resetTimer();


//oled functions
void welcomeOLED(int voltage, int rpmSet, bool direction, int rpmRead);
void OLEDvoltage(int voltage);
void OLEDrpmRead(int rpm);
void OLEDdir(bool direction);
void OLEDrpmSet(int rpm);

void setCombinedDACOutput(int inputValue);

void regulateRPMWithPID();

void turnOnRegulationPID(int setRPM);

#endif