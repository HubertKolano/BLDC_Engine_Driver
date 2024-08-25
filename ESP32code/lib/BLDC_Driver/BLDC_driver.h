#ifndef BLDC_DRIVER_H
#define BLDC_DRIVER_H

#include <Arduino.h>
#include <BLDC_driver.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_GFX.h>

#define BAUD_RATE 115200

// oled defines
#define LINE1 14
#define LINE2 32
#define LINE3 50
#define COLUMN1 10
#define COLUMN2 60
#define COLUMN3 90 

#define DAC1_PIN 25
#define DAC2_PIN 26 

//buttons names and timings
#define BUTTON1_PIN 18
#define BUTTON2_PIN 23
#define BUTTON3_PIN 19
#define BUTTON4_PIN 5
#define DEBOUNCE_TIME 50          
#define LONG_PRESS_TIME 1000
#define HOLD_PRESS_INTERVAL 100

#define ENGINE_CLOCKWISE false
#define ENGINE_COUNTERCLOCKWISE true


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

void welcomeOLED(int voltage, int rpm, bool direction);

void setCombinedDACOutput(int inputValue);


#endif