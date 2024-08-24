#include "BLDC_driver.h"

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
short line1_y = 14, line2_y = 32, line3_y = 50, column1_x = 5, column2_x = 60;

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

void myFunction(){
    Serial.println("Button action");
}

void updateOLED(int voltage, int rpm, bool direction) {
    u8g2.clearBuffer();
    u8g2.drawStr(column1_x, line1_y, "mV: ");
    u8g2.drawStr(column1_x, line2_y, "RPM: ");
    u8g2.drawStr(column1_x, line3_y, "DIR:");

    char voltageStr[5], rpmStr[5];
    sprintf(voltageStr, "%d", voltage);
    sprintf(rpmStr, "%d", rpm);

    // Display the updated values next to the labels
    u8g2.drawStr(column2_x, line1_y, voltageStr);     
    u8g2.drawStr(column2_x, line2_y, rpmStr);         
    u8g2.drawUTF8(column2_x, line3_y, direction ? "↻" : "↺");  

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
