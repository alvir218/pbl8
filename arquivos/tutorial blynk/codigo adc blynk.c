#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <esp8266wifi.h>
#include <BlynkSimpleEsp8266.h>

void LeituraDoSensor();
BlynkTimer timer;
const int ConversorAD = A0;  // Pino A0 = adc esp

char auth[] = "Token Gerado pelo Blynk APP";

char ssid[] = "nome da rede";
char pass[] = "senha da rede";

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, LeituraDoSensor); //recomendado pela documentacao do blynk, criar uma funcao e chamar com timer
}

void loop() {
  Blynk.run();
  //BlynkEdgent.run()
  timer.run(); //rodar o timer
}

void LeituraDoSensor(){
  // funcao interna de valor analogico
  int ValorSensor = analogRead(ConversorAD);
  Blynk.virtualWrite(V4, ValorSensor); // valor lido pelo sensor

}