#ifndef BLDC_WEBSITE_H
#define BLDC_WEBSITE_H

#include "BLDC_driver.h"
#include "BLDC_EngineRPM.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <LittleFS.h>

//password memory management
#define EEPROM_SIZE 512 
#define SSID_ADDR 0       
#define PASS_ADDR 256




//password management
void readWiFiCredentials(String &ssid, String &password);
void saveWiFiCredentials(const String &ssid, const String &password);

bool isWifiConnected();
IPAddress getWifiIP();
void initWIFI(String ssid, String password);
#endif
