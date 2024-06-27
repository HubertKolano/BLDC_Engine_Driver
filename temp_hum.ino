#include <stdio.h>
#include <stdbool.h>
#include <Wire.h>
#define DIGIT_COUNT 4   // ktora lampa 
#define DIGIT_VALUES 10
#define TOTAL_BUFFER (DIGIT_COUNT * DIGIT_VALUES)
#include <SHT31.h>
#define SHT31_ADDRESS   0x44
int dataPin = 23;       // Pin danych 74HC595 podłączony do cyfrowego pinu 23
int latchPin = 18;      // Pin zatrzasku 74HC595 podłączony do cyfrowego pinu 18
int clockPin = 5;       // Pin zegara 74HC595 podłączony do cyfrowego pinu 5
SHT31 sht;
int a,b,c,d;
int temp,hum;
uint32_t start;
uint32_t stop;

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
  //sht.begin();
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  sht.begin();
    uint16_t stat = sht.readStatus();
  Serial.print(stat, HEX);
  Serial.println();
 //Serial.println("Start");
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);
//  pinMode(SWITCH_PIN, INPUT_PULLUP);  // Konfiguracja pinu przełącznika jako wejściowy z rezystorem pull-up
  //setCpuFrequencyMhz(20);
}

// Funkcja czyszcząca cyfrę w buforze
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
// Funkcja czyszcząca wyświetlacz
void clearDisplay() {
  for (int i = 0; i < DIGIT_COUNT; i++) {
    clear_digit(i);
  }
  send_to_shift_registers();
}

 int neon (){
   Serial.println("FUKCJA NEON START "); // Dodanie komunikatu debugującego
    digitalWrite(14, LOW);
    digitalWrite(12, LOW);
    delay (500);
    digitalWrite(12, HIGH);
    digitalWrite(14, HIGH);
    delay (500);
    Serial.println("FUKCJA NEON KONIEC ");
  }


int abc() {
    set_digit(1, 8);
    print_buf();
    send_to_shift_registers();
    delay(1000); // Delay for 1 second to observe the change
    neon(); // Dodajemy funkcję neon tutaj, aby sprawdzić, czy działa w tym kontekście
}

void displayTemperatureAndHumidity() {
    start = micros();
  sht.read(false);         //  default = true/fast       slow = false
  stop = micros();
      int temp=(sht.getTemperature());
    int hum=(sht.getHumidity());

//        a= temp / 10;
//        b= temp % 10;
//        c= hum / 10;
//        d= hum % 10;
        set_digit(1,temp / 10 );   // Dziesiątki temperatury
        set_digit(2,temp % 10);   // Jedności temperatury
        set_digit(3,hum / 10 );    // Dziesiątki wilgotności
        set_digit(4,hum % 10 );    // Jedności wilgotności
        
        send_to_shift_registers();
    }
void termometr() {
    Serial.println("Loop start"); // Dodanie komunikatu debugującego
    //abc();
   //neon();
    //displayTemperatureAndHumidity();
  start = micros();
  sht.read(false);         //  default = true/fast       slow = false
  stop = micros();

  Serial.println(sht.getTemperature(), 1);
  Serial.println(sht.getHumidity());

    int temp=(sht.getTemperature());
    int hum=(sht.getHumidity());
    int t1,t2,h1,h2 ;
        t1= temp / 10;
        t2= temp % 10;
        h1= hum / 10;
        h2= hum % 10;
        set_digit(1,t1 );   // Dziesiątki temperatury
        set_digit(2,t2 );   // Jedności temperatury
        set_digit(3,h1 );    // Dziesiątki wilgotności
        set_digit(4,h2 );    // Jedności wilgotności
    Serial.println(t1);
    Serial.println(t2);
    Serial.println(h1);
    Serial.println(h2);
        
        send_to_shift_registers();

  delay(1000);
    }

// Funkcja pętli głównej
void loop() {
termometr();

}
