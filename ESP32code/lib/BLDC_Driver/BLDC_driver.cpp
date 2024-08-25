#include "BLDC_driver.h"


//screen declarations
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//button declarations
Button buttons[] = {
    {BUTTON1_PIN, button1Press, false, false, 0, 0, 0},
    {BUTTON2_PIN, button2Press, false, false, 0, 0, 0},
    {BUTTON3_PIN, button3Press, false, false, 0, 0, 0},
    {BUTTON4_PIN, button4Press, false, false, 0, 0, 0} 
};

//timer declarations
hw_timer_t *timer = NULL;      
unsigned long elapsedTime = 0; 
unsigned long lastResetTime = 0; 

void initOLED() {
    Wire.begin();

    u8g2.begin();
    
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_cu12_h_symbols); 

    u8g2.sendBuffer();
}

void initSerial(){
    Serial.begin(115200);
    Serial.println("Hello World!");
}


const int numButtons = sizeof(buttons) / sizeof(buttons[0]);

void initButtons() {

    for (int i = 0; i < numButtons; i++) {
        pinMode(buttons[i].pin, INPUT_PULLUP); 
    }
}

void checkButtons() {

    for (int i = 0; i < numButtons; i++) {
        handleButtonPress(buttons[i]);
    }
}

void handleButtonPress(Button &button) {
    unsigned long currentTime = millis();
    bool buttonPressed = digitalRead(button.pin) == LOW; 

    if (buttonPressed && !button.isPressed && currentTime - button.lastPressTime > DEBOUNCE_TIME) {
        button.isPressed = true;
        button.pressStartTime = currentTime;
        button.longPressTriggered = false;
        button.lastLongPressActionTime = currentTime;
        button.lastPressTime = currentTime;
        // detection of short press
            button.PressCallback();
    }

    if (buttonPressed && button.isPressed) {
            // Long press detected
        if (!button.longPressTriggered && currentTime - button.pressStartTime > LONG_PRESS_TIME) {
            button.PressCallback();
            button.longPressTriggered = true;  // Prevent further long press callbacks until released
        } else if (button.longPressTriggered && currentTime - button.lastLongPressActionTime > HOLD_PRESS_INTERVAL) {
            button.PressCallback();
            button.lastLongPressActionTime = currentTime;
        }
    }
        //detect button release
    if (!buttonPressed && button.isPressed && currentTime - button.lastPressTime > DEBOUNCE_TIME) {
        button.isPressed = false;
        button.lastPressTime = currentTime;

    }
}

void button1Press(){
    Serial.println("Button 1 press!");
}
void button2Press(){
    Serial.println("Button 2 press!");
}
void button3Press(){
    Serial.println("Button 3 press!");
}
void button4Press(){
    Serial.println("Button 4 press!");
}

//count time increment every interrupt (which is) 100ms
void IRAM_ATTR onTimer() {
  elapsedTime += 100;
}

void setupTimer() {
  timer = timerBegin(0, 80, true);             // Use timer 0, 80Mhz with prescaler of 80 results in 1us tick
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000, true);        // Set alarm to 100000us (100ms)
  timerAlarmEnable(timer);
}

// Function to check if the specified amount of time has passed since last reset (also resets)
bool checkElapsedTime(unsigned long interval) {
  if (elapsedTime - lastResetTime >= interval) {
    lastResetTime = elapsedTime;
    return true;
  }
  return false;
}

// Function to reset the timer manually
void resetTimer() {
  lastResetTime = elapsedTime;
}

void welcomeOLED(int voltage, int rpm, bool direction) {
    u8g2.clearBuffer();
    u8g2.drawStr(0 , LINE1 , ">");
    u8g2.drawStr(COLUMN1 , LINE1 , "mV: ");
    u8g2.drawStr(COLUMN1 , LINE2 , "RPM: ");
    u8g2.drawStr(COLUMN1 , LINE3 , "DIR:");

    char voltageStr[5], rpmStr[5];
    sprintf(voltageStr, "%d", voltage);
    sprintf(rpmStr, "%d", rpm);

    u8g2.drawStr(COLUMN2 , LINE1, voltageStr);     
    u8g2.drawStr(COLUMN2 , LINE2, rpmStr);         
    u8g2.drawUTF8(COLUMN2 ,LINE3, direction ? "↻" : "↺");  

    u8g2.drawStr(COLUMN3 , LINE1, "set");    

    u8g2.sendBuffer();
}

void setCombinedDACOutput(int inputValue) {
    // input check
    if (inputValue < 0 || inputValue > 5000) {
        Serial.println("Error: Input out of range. Value should be between 0 and 5000.");
        return;
    }
    
    //mapping values, maxes out one dac before usses second one
    int dac1Voltage, dac2Voltage;

    if (inputValue <= 2500) {
        dac1Voltage = inputValue;
        dac2Voltage = 0;
    } else {
        dac1Voltage = 2500;
        dac2Voltage = inputValue - 2500;
}

    int dac1Value = map(dac1Voltage, 0, 2500, 0, 255);
    int dac2Value = map(dac2Voltage, 0, 2500, 0, 255);

    dacWrite(DAC1_PIN, dac1Value);
    dacWrite(DAC2_PIN, dac2Value);

    // Debugging info for UART
    Serial.printf("DAC written, Input: %d [mV] | DAC1: %d bits | DAC2: %d bits\t\n", inputValue, dac1Value, dac2Value);
}
