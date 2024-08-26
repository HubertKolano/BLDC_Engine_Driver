#include <Arduino.h>

const int analogPin = 34;      // Analog input pin
const int pwmPin = 14;         // Pin for PWM output
const int pwmChannel = 0;      // PWM channel (0-15 on ESP32)
const int pwmResolution = 8;   // PWM resolution (8-bit: 0-255)
const int pwmBaseFreq = 6000;  // Base frequency for PWM (max frequency)

unsigned long previousMillis = 0; // To store last update time
const unsigned long interval = 2000; // Update interval of 2sec

void setup() {
  Serial.begin(115200);

  // Setup PWM on the specified pin
  ledcSetup(pwmChannel, pwmBaseFreq, pwmResolution);
  ledcAttachPin(pwmPin, pwmChannel);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read the analog value (range from 0 to 4095)
    int analogValue = analogRead(analogPin);
    
    // Map the analog value to a frequency range from 0 to 6000 Hz
    int mappedFreq = map(analogValue, 0, 4095, 0, 6000);

    // Output the frequency using PWM
    if (mappedFreq > 0) {
      ledcWriteTone(pwmChannel, mappedFreq); // Set PWM frequency
    } else {
      ledcWriteTone(pwmChannel, 0); // Stop the signal if frequency is 0
    }

    // Output for debugging
    Serial.print("Analog Value: ");
    Serial.print(analogValue);
    Serial.print(" - Mapped Frequency: ");
    Serial.println(mappedFreq);
  }
}