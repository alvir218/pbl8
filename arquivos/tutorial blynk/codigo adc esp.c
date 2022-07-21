#include <Arduino.h>
#include <esp8266wifi.h>

const int ConversorAD = A0;  // Pino A0 = adc esp

int ValorSensor = 0;  // valor lido pelo sensor

void setup() {
  Serial.begin(9600);
}

void loop() {
  // funcao interna de valor analogico
  ValorSensor = analogRead(ConversorAD);
 
  // printar na serial
  Serial.print("Valor = ");
  Serial.print(ValorSensor);
  
  delay(1000);
}