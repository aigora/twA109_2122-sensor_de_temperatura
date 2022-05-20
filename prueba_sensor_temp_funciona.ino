#include "DHT.h"
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
#include <LiquidCrystal.h> 
const int LedPin = 11;
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
const int DHTPin = 10;     // what digital pin we're connected to
//DHT dht(DHTPin, DHTTYPE);
DHT dht(10, DHT11); 
LiquidCrystal lcd(8, 7, 6, 5, 4, 3, 2); 

void setup() {
   Serial.begin(9600);
   Serial.println("DHTxx test!");
   lcd.begin(16, 1);
   dht.begin();
   pinMode(LedPin, OUTPUT);
}
void loop() {
   // Wait a few seconds between measurements.
   delay(200);
   // Reading temperature or humidity takes about 250 milliseconds!
   float h = dht.readHumidity();
   float t = dht.readTemperature();
   if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
   }
   Serial.print("Humidity: ");
   Serial.print(h);
   
   Serial.print(" %\t");
   Serial.print("Temperature: ");
   Serial.print(t);
   Serial.print(" *C ");
   lcd.print("Tem:" + String(t, 1) + "C  ");
    
   if(t<=22.4)
   {
   digitalWrite(LedPin,HIGH);
   }
   else
   digitalWrite(LedPin,LOW);
}
