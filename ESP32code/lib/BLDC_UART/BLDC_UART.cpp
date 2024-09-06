#include "BLDC_UART.h"

bool choiceMade = false;

/* SERIAL INTERFACE CODE*/
void initSerial(){
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
    sendToBothUarts("Welcome to the BLDC interface!\n");
}

void sendToBothUarts(String message) {
  Serial.print(message);  // UART0
  Serial2.print(message); // UART2
}

void serialMenuMessage(){
    sendToBothUarts("Please choose an option:\n");
    sendToBothUarts("1 - Input voltage value\n");
    sendToBothUarts("2 - Input RPM set value\n");
    sendToBothUarts("3 - Change engine direction\n");
    sendToBothUarts("4 - Check voltage\n");
    sendToBothUarts("5 - Check RPM read from engine\n");
    sendToBothUarts("6 - Check Wi-Fi connection\n");
    sendToBothUarts("7 - Enter SSID and password\n");
    sendToBothUarts("8 - Show Menu\n");
}

String readFromBothUarts() {
  String input = "";

  if (Serial.available()) {
    input = Serial.readStringUntil('\0');
  }

  if (input.length() == 0 && Serial2.available()) {
    input = Serial2.readStringUntil('\0');
  }
  input.trim();
  return input;
}

void handleChoice1() {
    voltageDACS = readFromBothUarts().toInt();
    setCombinedDACOutput(voltageDACS);
    turnOffRegulationPID();
}
void handleChoice2(){
    float input = readFromBothUarts().toFloat();
    turnOnRegulationPID(input * 50);
}

void handleChoice3() {
  changeDirection();
}


void handleChoice4(){
    sendToBothUarts(String(voltageDACS));
}
void handleChoice5(){
    formatRPM(engineReadRPM);
}
void handleChoice6(){
    isWifiConnected() ? sendToBothUarts("Wifi not connected") : sendToBothUarts("Wifi connected");
    saveWiFiCredentials("giga18084", "net191688");
}

void handleChoice7() {
  String string1 = readFromBothUarts();
  String string2 = readFromBothUarts();
  saveWiFiCredentials(string1, string2);

}
void handleChoice8(){
    serialMenuMessage();
}
void checkUart(){
    
    String userInput = readFromBothUarts();
    if (userInput.length() > 0) {
        int intValue = userInput.toInt();
        Serial.print("\nYou selected: " + userInput + "\n");

        switch (intValue)
        {
        case 1:
            handleChoice1();
            break;
        case 2:
            handleChoice2();
            break;
        case 3:
            handleChoice3();
            break;
        case 4:
            handleChoice4();
            break;
        case 5:
            handleChoice5();
            break;
        case 6:
            handleChoice6();
            break;
        case 7:
            handleChoice7();
            break; 
        case 8:
            handleChoice8();
            break; 
        default:
        Serial.print("Invalid choice! Please select number 1 to 7\n");
            break;
    }
  }
}




