#include "BLDC_UART.h"

bool choiceMade = false;

/* SERIAL INTERFACE CODE*/

//both uarts functions
void initSerial(){
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
    Serial2.onReceive(onReceive);
    Serial.print("BLDC driver starting up...\n");
    Serial2.print("BLDC driver starting up...\n");
}

void sendToBothUarts(String message) {
  Serial.print(message);  // UART0
  Serial2.print(message); // UART2
}


//UART0 functions
void serial0MenuMessage(){
    Serial.print("Please choose an option:\n");
    Serial.print("1 - Input voltage value\n");
    Serial.print("2 - Input RPM set value\n");
    Serial.print("3 - Change engine direction\n");
    Serial.print("4 - Check voltage\n");
    Serial.print("5 - Check RPM read from engine\n");
    Serial.print("6 - Check Wi-Fi connection\n");
    Serial.print("7 - Enter SSID and password\n");
    Serial.print("8 - Show Menu\n");
}

String readFromUart0() {
  String input = "";

  if (Serial.available()) {
    input = Serial.readStringUntil('\0');
  }
  return input;
}

void handleChoice1() {
    voltageDACS = readFromUart0().toInt();
    setCombinedDACOutput(voltageDACS);
    turnOffRegulationPID();
}
void handleChoice2(){
    float input = readFromUart0().toFloat();
    turnOnRegulationPID(input * ENGINE_TORQUE);
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
  String string1 = readFromUart0();
  String string2 = readFromUart0();
  saveWiFiCredentials(string1, string2);

}
void handleChoice8(){
    serial0MenuMessage();
}
void checkUart(){
    
    String userInput = readFromUart0();
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

volatile bool Uart2Ready = false;
char messageBuffer[BUFFER_SIZE];
volatile uint8_t bufferIndex = 0;  

//UART2 functions
void IRAM_ATTR onReceive() {
  while (Serial2.available()) {
        char receivedChar = Serial2.read();   // Read incoming character
    
        if (receivedChar == '\n' || receivedChar == '\0' || receivedChar == '\r') {
            messageBuffer[bufferIndex] = '\0';   // Null-terminate the string
            Uart2Ready = true;                    // Set the flag to indicate message is ready
            bufferIndex = 0;         
            Serial2.print("\n");          
        } else {
            // Store the character in the buffer if there's space
            if (bufferIndex < BUFFER_SIZE - 1) {
                messageBuffer[bufferIndex++] = receivedChar;
                Serial2.print(receivedChar); 
            }
        }
}
}


void checkUarts(){
    if(Uart2Ready){
        parseSCPICommand(messageBuffer);
        Uart2Ready = false;

    }
}

//UART2 MENU
void parseSCPICommand(char *scpiCommand) {
    float floatValue;
    char strValue1[50], strValue2[50];
    
    // Check and parse the command
    if (sscanf(scpiCommand, "SET:VOLTAGE %d", &voltageDACS) == 1) {
        setCombinedDACOutput(voltageDACS);  
        turnOffRegulationPID();
    } 
    else if (sscanf(scpiCommand, "SET:RPM %f", &floatValue) == 1) {
        if (floatValue > ENGINE_MAX_RPM/ENGINE_TORQUE || floatValue < 0 ){
            Serial2.println("Error: RPM outside range");
        }
        turnOnRegulationPID(floatValue * ENGINE_TORQUE);  
    } 
    else if (sscanf(scpiCommand, "SET:WIFI %s %s", strValue1, strValue2) == 2) {
        saveWiFiCredentials(strValue1, strValue2);
    } 
    else if (strcmp(scpiCommand, "SET:DIRECTION") == 0) {
        changeDirection(); 
    } 
    else if (strcmp(scpiCommand, "GET:WIFI") == 0) {
        isWifiConnected() ? Serial2.println(getWifiIP()) : Serial2.print("Wifi not connected\n");
    }
    else if (strcmp(scpiCommand, "GET:VOLTAGE") == 0) {
        Serial2.println(voltageDACS); 
    } 
    else if (strcmp(scpiCommand, "GET:RPM") == 0) {
        Serial2.println(formatRPM(engineReadRPM)); 
    }
    else {
        Serial2.println("Error: Invalid SCPI command format.");
        Serial.println("Error: Invalid SCPI command format.");
    }
}