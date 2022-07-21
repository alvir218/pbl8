#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <esp8266wifi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "Token Gerado pelo Blynk APP";

char ssid[] = "nome da rede";
char pass[] = "senha da rede";

void setup() 
{
   Serial.begin(9600);
   //Serial.println("Estado do led");
   //pinMode(D1,OUTPUT);

   Blynk.begin(auth, ssid, pass);
}
void loop() 
{ 
    /*digitalWrite(D1, HIGH);
    Serial.println("Ligado");
    delay(2000);
    digitalWrite(D1, LOW);
    Serial.println("Desligado");
    delay(2000); */
    Blynk.run();
}