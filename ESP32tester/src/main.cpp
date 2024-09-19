#include <Arduino.h>

#define ANALOG_PIN 34
#define PWM_PIN 14
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 8
#define PWM_BASE_FREQ 2100
#define TIME_INTERVAL 100 // 0,1sec update time
unsigned long previousMillis = 0; // To store last update time

void setup() {
  Serial.begin(115200);

  // Setup PWM on the specified pin
  if (!ledcSetup(PWM_CHANNEL, PWM_BASE_FREQ, PWM_RESOLUTION)){
    Serial.println("Failed to setup PWM");
  }
  ledcAttachPin(PWM_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 64);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= TIME_INTERVAL) {
    previousMillis = currentMillis;
    // Read the analog value (range from 0 to 4095)
    int analogValue = analogRead(ANALOG_PIN);
    
    int mappedFreq = map(analogValue, 0, 4095, 0, PWM_BASE_FREQ);

    if (mappedFreq > 0) {
    ledcChangeFrequency(PWM_CHANNEL, mappedFreq, PWM_RESOLUTION);
    ledcWrite(PWM_CHANNEL, 64);
    } else {
    ledcWrite(PWM_CHANNEL, 0);
    }
    // Output for debugging
    Serial.printf("Analog Value: %d - Mapped Frequency: %d\n" ,analogValue, mappedFreq);

  }
}