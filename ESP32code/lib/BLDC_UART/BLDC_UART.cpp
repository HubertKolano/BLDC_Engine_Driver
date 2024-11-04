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