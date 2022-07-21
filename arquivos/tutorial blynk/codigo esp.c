#include <Arduino.h>
#include <esp8266wifi.h>

void setup() 
{
   Serial.begin(9600);
   Serial.println("Estado do led");
   pinMode(D1,OUTPUT);
}
void loop() 
{ 
    digitalWrite(D1, HIGH);
    Serial.println("Ligado");
    delay(2000);
    digitalWrite(D1, LOW);
    Serial.println("Desligado");
    delay(2000);
}