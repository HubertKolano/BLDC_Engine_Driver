#include <stdio.h>
#include <stdbool.h>
#include <Wire.h>
#define DIGIT_COUNT 4   // ktora lampa 
#define DIGIT_VALUES 10
#define TOTAL_BUFFER (DIGIT_COUNT * DIGIT_VALUES)
int dataPin = 23;       // Pin danych 74HC595 podłączony do cyfrowego pinu 23
int latchPin = 18;      // Pin zatrzasku 74HC595 podłączony do cyfrowego pinu 18
int clockPin = 5;       // Pin zegara 74HC595 podłączony do cyfrowego pinu 5
//////////////////////////
#include <WiFi.h>
#include "time.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
const char* ssid     = "NTP";
const char* password = "12345678";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);  // UTC+1 (3600 sekund)
int g1, g2, m1, m2;
/////////////////////
typedef struct {
    bool value[DIGIT_VALUES];
} NixieDigit; // Struktura reprezentująca pojedynczą cyfrę Nixie

typedef union {
    bool buf[TOTAL_BUFFER];
    NixieDigit digit[DIGIT_COUNT];
} NixieBuffer; // Unia przechowująca bufor Nixie

NixieBuffer display = {}; // Inicjalizacja bufora Nixie

void setup() {
  // Inicjalizacja pinów
   Serial.begin(9600);
  ///////////////////////////
    
      Wire.begin();
      Wire.setClock(100000);
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Łączenie z Wi-Fi...");
      }
      Serial.println("Połączono z Wi-Fi");
  // Rozpoczęcie synchronizacji czasu
  timeClient.begin();
  //////////////////////
  Serial.println("Start");
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);
}
int clear_digit(int val)
{
    if((0 > val) || (DIGIT_VALUES < val))
    {
      //  Serial.write("clear_digit - wrong value %d\n", val); // Komunikat o błędzie
        return -1;
    }
    else
    {
        for(int i = 0; i < DIGIT_VALUES; i++)
        {
            display.digit[val].value[i] = false; // Ustawienie wszystkich wartości na false
        }
    }
    return 0;
}

// Funkcja ustawiająca cyfrę w buforze
int set_digit(int digit, int value)
{   
    digit--; // Konwersja numeracji na 0-3
    if((0 > digit) || (DIGIT_COUNT < digit))
    {  
     //   Serial.write("set_digit - wrong digit %d\n", digit); // Komunikat o błędzie
        return -1;
    }
    else if((0 > value) || (DIGIT_VALUES < value))
    {
      //  Serial.write("set_digit - wrong value %d\n", value); // Komunikat o błędzie
        return -2;
    }
    else if(0 != clear_digit(digit))
    {
      //  Serial.write("set_digit - unable to clear digit\n"); // Komunikat o niepowodzeniu w czyszczeniu cyfry
        return -3;
    }
    else
    {
        // Ustawienie odpowiedniej wartości na true w cyfrze
        //(DIGIT_VALUES - value) % DIGIT_VALUES
        display.digit[digit].value[(DIGIT_VALUES - value) % DIGIT_VALUES] = true;
       // display.digit[digit].value[(value + DIGIT_VALUES - 1) % DIGIT_VALUES] = true;
    }
    return 0;
}

// Funkcja wyświetlająca zawartość bufora
int print_buf()
{
    for (int i = 0; i < 40; i++)
    {
        if(i%10==0)
        {
        //    Serial.write("|"); // Co 10 element wyświetlaj |
        }
        //Serial.write("%d", display.buf[i]); // Wyświetlanie zawartości bufora
    }
   // Serial.write("\n");
    return 0;
}

// Funkcja wysyłająca zawartość bufora do rejestrów 74HC595
void send_to_shift_registers() {
  char txt [100]= {};
   digitalWrite(latchPin, LOW); // Wyłączamy zatrzask
   digitalWrite(clockPin, LOW); // Włączamy zegar
   
   for(int i = 0; i < 5 ; i++) { // Pętla od pierwszego do ostatniego bitu w buforze
        for(int j = 0; j < 8; j++) { // Pętla dla każdego bitu w bajcie
            delayMicroseconds(10); 
            digitalWrite(clockPin, LOW); // Wyłączamy zegar
            sprintf(txt,"%d", (display.buf[i*8 + j]) & 0x01);
            Serial.write(txt);
            //Serial.write("%d", (display.buf[i*8 - j]) & 0x01);
            digitalWrite(dataPin, (display.buf[i*8 + j]) & 0x01); // Ustawiamy bit danych
            delayMicroseconds(10); 
            digitalWrite(clockPin, HIGH); // Włączamy zegar
        }
    }
Serial.write("XX\n");   
digitalWrite(latchPin, HIGH); // Wyłączamy zatrzask    

}

void clearDisplay() {
  for (int i = 0; i < DIGIT_COUNT; i++) {
    clear_digit(i);
  }
  send_to_shift_registers();
}

void zegar()
{
   timeClient.update();
     int hours = timeClient.getHours();
     int minutes = timeClient.getMinutes();

  // Dziesiątki i jedności godzin
  g1 = hours / 10;
  g2 = hours % 10;
  m1 = minutes / 10;
  m2 = minutes % 10;
        set_digit(1, g1);
        set_digit(2, g2); // Update second digit with current value of digit variable
        set_digit(3, m1);
        set_digit(4, m2);
  
        print_buf(); // nie działa bez funkcji send_to_
        send_to_shift_registers();
          Serial.print("Godziny: ");
  Serial.print(g1);
  Serial.print(g2);
  Serial.print(" Minuty: ");
  Serial.print(m1);
  Serial.println(m2);
  //Serial.println(hours, minutes);
 delay(1000);  
  
  
  }

void loop() {

zegar();
}
