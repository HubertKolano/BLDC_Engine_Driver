#include "BLDC_driver.h"

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);



Button buttons[] = {
    {BUTTON1_PIN, button1Press, false, false, 0, 0, 0},
    {BUTTON2_PIN, button2Press, false, false, 0, 0, 0},
    {BUTTON3_PIN, button3Press, false, false, 0, 0, 0},
    {BUTTON4_PIN, button4Press, false, false, 0, 0, 0} 
};

void initOLED() {
    Wire.begin();

    u8g2.begin();
    
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_cu12_h_symbols); 

    u8g2.sendBuffer();
}

void initSerial(){
    Serial.begin(921600);
    Serial.println("Hello World!");
}


const int numButtons = sizeof(buttons) / sizeof(buttons[0]);

void initButtons() {
    // Set pin modes for each button
    for (int i = 0; i < numButtons; i++) {
        pinMode(buttons[i].pin, INPUT_PULLUP);  // Use pull-up resistors
    }
}

void checkButtons() {
    unsigned long currentTime = millis();

    // Loop through each button and check its state
    for (int i = 0; i < numButtons; i++) {
        handleButtonPress(buttons[i]);
    }
}

void handleButtonPress(Button &button) {
    unsigned long currentTime = millis();
    bool currentState = digitalRead(button.pin) == LOW;  // Button is pressed when LOW

    if (currentState && !button.isPressed && currentTime - button.lastPressTime > DEBOUNCE_TIME) {
        button.isPressed = true;
        button.pressStartTime = currentTime;
        button.longPressTriggered = false;
        button.lastLongPressActionTime = currentTime; // Initialize last action time
        button.lastPressTime = currentTime;
    }

    if (currentState && button.isPressed) {
        // Button is being held down
        if (!button.longPressTriggered && currentTime - button.pressStartTime > LONG_PRESS_TIME) {
            // Long press detected
            button.PressCallback();
            button.longPressTriggered = true;  // Prevent further long press callbacks until released
        } else if (button.longPressTriggered && currentTime - button.lastLongPressActionTime > HOLD_PRESS_INTERVAL) {
            button.PressCallback();
            button.lastLongPressActionTime = currentTime;
        }
    }

    if (!currentState && button.isPressed && currentTime - button.lastPressTime > DEBOUNCE_TIME) {
        // Button was just released
        button.isPressed = false;
        button.lastPressTime = currentTime;

        if (!button.longPressTriggered) {
            // Short press detected (only trigger if long press hasn't been triggered)
            button.PressCallback();
        }
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


// void myFunction(Button &button) {
//     switch (button.pin){
//         case BUTTON1_PIN:  Serial.println("Button on pin 1"); break;
//         case BUTTON2_PIN:  Serial.println("Button on pin 2"); break;
//         case BUTTON3_PIN:  Serial.println("Button on pin 3"); break;
//         case BUTTON4_PIN:  Serial.println("Button on pin 4"); break;
//     }
// }




void updateOLED(int voltage, int rpm, bool direction) {
    u8g2.clearBuffer();
    u8g2.drawStr(COLUMN1_X , LINE1_Y , "mV: ");
    u8g2.drawStr(COLUMN1_X , LINE2_Y , "RPM: ");
    u8g2.drawStr(COLUMN1_X , LINE3_Y , "DIR:");

    char voltageStr[5], rpmStr[5];
    sprintf(voltageStr, "%d", voltage);
    sprintf(rpmStr, "%d", rpm);

    // Display the updated values next to the labels
    u8g2.drawStr(COLUMN2_X , LINE1_Y, voltageStr);     
    u8g2.drawStr(COLUMN2_X , LINE2_Y, rpmStr);         
    u8g2.drawUTF8(COLUMN2_X ,LINE3_Y, direction ? "↻" : "↺");  

    u8g2.sendBuffer();
}

void setCombinedDACOutput(int inputValue) {
  // Check if the input is within the allowed range
  if (inputValue < 0 || inputValue > 5000) {
    Serial.println("Error: Input out of range. Value should be between 0 and 5000.");
    return;
  }
  
  int outputVoltage = map(inputValue, 0, 5000, 0, 6600);

  int dac1Voltage = outputVoltage / 2; 
  int dac2Voltage = outputVoltage - dac1Voltage; 

  int dac1Value = map(dac1Voltage, 0, 3300, 0, 255);
  int dac2Value = map(dac2Voltage, 0, 3300, 0, 255);

  // Output the values to the DAC pins
  dacWrite(DAC1_PIN, dac1Value);
  //dacWrite(DAC2_PIN, dac2Value);

  // Debugging info for UART
  Serial.printf("Input: %d [mV] | DAC1: %d bits | DAC2: %d bits\n", inputValue, dac1Value, dac2Value);
}
