//desmembrado

//import do platformio
#include <Arduino.h> 
#include <esp8266wifi.h>
#include "DHT.h"
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>

#define RELAY_FECHAR 2 // GPIO que está ligado o LED - GPOI4 d4
#define RELAY_ABRIR 5 // GPIO5 //testar
#define PIN_DHT 4 // GPIO que está ligado o pino do sensor - na placa esta na porta  d2 que esta ligando o pino do sensor
#define DHTTYPE DHT11 // DHT 11
#define FIM_DE_CURSO_FECHAR  16    //Fim curso GPIO16 = D0
#define FIM_DE_CURSO_ABRIR 14 //gpio14 //d5

/* Configura os tópicos do MQTT */
#define TOPIC_SUBSCRIBE_SWITCH    "Uninter/switch"
#define TOPIC_PUBLISH_TEMPERATURE "Uninter/temperatura"
#define TOPIC_PUBLISH_HUMIDITY    "Uninter/humidade"

//#define PUBLISH_DELAY 2000   // Atraso da publicação (2 segundos)

#define ID_MQTT "c31f731b-801d-4ec8-ad68-e37e588758fe" // id mqtt (para identificação de sessão)

// IMPORTANTE: Este deve ser único no broker, ou seja, se um client MQTT
// tentar entrar com o mesmo id de outro já conectado ao broker,
// o broker irá fechar a conexão de um deles.

const char *SSID = "iPhone de Drica"; // SSID / nome da rede WI-FI que deseja se conectar
const char *PASSWORD = "123adri123";        // Senha da rede WI-FI que deseja se conectar

// URL do broker MQTT que se deseja utilizar
const char *BROKER_MQTT = "broker.hivemq.com";

int BROKER_PORT = 1883; // Porta do Broker MQTT

//unsigned long publishUpdate;

static char strTemperature[10] = {0};
static char strHumidity[10] = {0};

//variaveis com status e contexto

static char fim_de_curso_fechar = 0;//variavel do fim de curso
static char fim_de_curso_abrir = 0;
static float humidade;
static float temperatura;
//static unsigned int escuro; //implementar
//static char molhado;        //implementar
static char abrir_janela;
static char fechar_janela;
static char janela_aberta;
static char janela_fechada;
static char janela_abrindo;
static char janela_fechando;
static char mqtt_conectado;
static char wifi_conectado;
static char emperrado;

WiFiClient espClient;         // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
DHT dht(PIN_DHT, DHTTYPE); //instancia o dht11

//header
void abertura_janela(void);
void callbackMQTT(char *topic, byte *payload, unsigned int length);
void connect_mqtt(void);
void connect_wifi(void);
void controlador_da_janela(void);
void fechamento_janela(void);
float getTemperature(void);
float getHumidity(void);
void initMQTT(void);
void monitor_serial(void);
void publicar_mqtt(void);


void abertura_janela(void){
  
  if(abrir_janela && janela_fechada){
    janela_abrindo = 1;
    digitalWrite(RELAY_ABRIR, HIGH);
    delay(12000);
    fim_de_curso_abrir = digitalRead(FIM_DE_CURSO_ABRIR);
    if(fim_de_curso_abrir == 0){
      janela_abrindo = 0;
      janela_aberta = 1;
    }
    else{
      emperrado = 1;
    }
    digitalWrite(RELAY_ABRIR, LOW);
  }
  
}



/* Função de callback  esta função é chamada toda vez que uma informação
   de um dos tópicos subescritos chega */
void callbackMQTT(char *topic, byte *payload, unsigned int length)
{
  String msg;

  //msg = (char*)malloc(sizeof(char));

  // Obtem a string do payload recebido
  for (unsigned int i = 0; i < length; i++) {
    char c = (char)payload[i];
    msg += c;
  }
  
  //memoriza a ordem para abrir ou fechar janela
  if (msg[0] == '1') {
    abrir_janela = 1;
  }

  if (msg[0] == '0') {
    fechar_janela = 1;
  }
}



/* Reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
   em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito a cada 2 segundos*/
void connect_mqtt(void)
{
  if (!MQTT.connected()) {
    //flags para mensagens de status mqtt
    mqtt_conectado = 0; 
    if (MQTT.connect(ID_MQTT)) {
      mqtt_conectado = 1;      
      MQTT.subscribe(TOPIC_SUBSCRIBE_SWITCH);
      // Keep-alive da comunicação com broker MQTT
      MQTT.loop();
      return;
    }
  }
}


//confere se esta conectado, se nao estiver tenta reconectar
void connect_wifi(void){
  // se já está conectado a rede WI-FI, nada é feito.
  // Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED){
    wifi_conectado = 1;
    return;
  }else{
    wifi_conectado = 0;
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
  }
}



void controlador_da_janela(void){
  


  if(temperatura < 18.0 
    && janela_aberta 
    && !janela_abrindo 
    && !janela_fechando){
    fechamento_janela();
  }

  if(temperatura > 23.0 
    && janela_fechada 
    && !janela_abrindo 
    && !janela_fechando
    //&& !molhado
    ){
    abertura_janela();
  }

  //implementar fechar quando chovendo

  //implementar fechar quando escuro

}


void fechamento_janela(void){
  if(fechar_janela && janela_aberta){
    janela_fechando = 1;
    digitalWrite(RELAY_FECHAR, HIGH);
    delay(12000);
    fim_de_curso_fechar = digitalRead(FIM_DE_CURSO_FECHAR);
    if(fim_de_curso_fechar == 0){
      janela_fechando = 0;
      janela_fechada = 1;
    }
    else{
      emperrado = 1;
    }
    digitalWrite(RELAY_FECHAR, LOW);
  }
}



/* Faz a leitura da temperatura (sensor DHT11)
   Retorno: temperatura (graus Celsius) */
float getTemperature(void)
{
  //para dht11 - se a temperatura nao for numero (is num) retorne -99.99
  temperatura = dht.readTemperature();
  if (!(isnan(temperatura)))
    return temperatura;
  else
    return -99.99;
}



/* Faz a leitura da umidade relativa do ar (sensor dht11)
   Retorno: umidade (0 - 100%) */
float getHumidity(void) {
  humidade = dht.readHumidity();
  if (!(isnan(humidade)))
    return humidade;
  else
    return -99.99;
}



/* Inicializa os parâmetros de conexão MQTT(endereço do broker, porta e seta
  função de callback) */
void initMQTT(void)
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT); // Informa qual broker e porta deve ser conectado
  MQTT.setCallback(callbackMQTT);           // Atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}



void monitor_serial(void){


  //contexto do wifi
  if (WiFi.status() == WL_CONNECTED){
    Serial.println();
    Serial.print("conectado em: ");
    Serial.print(SSID);
    Serial.println("   ip: ");
    Serial.println(WiFi.localIP());
  }
  else{
    Serial.println();
    Serial.print("wifi desconectado");
    Serial.println();
  }
  
  delay(500);


  //contexto da janela
  if(abrir_janela){
    Serial.println("abrir janela");
  }

  if(janela_abrindo){
    Serial.println("janela abrindo");
  }

  if(janela_aberta){
    Serial.println("janela aberta");
  }

  if(fechar_janela){
    Serial.println("fechar janela");
  }

  if(janela_fechando){
    Serial.println("janela fechando");
  }

  if(janela_fechada){
    Serial.println("janela fechada");
  }

  if(emperrado){
    Serial.println("janela emperrada");
  }

  delay(500);
  

  //status da conexao mqtt
  if(!MQTT.connected()){
    Serial.print("conectando mqtt em: ");
    Serial.println(BROKER_MQTT);
  }
  else{
    Serial.print("mqtt conectado em: ");
    Serial.println(BROKER_MQTT);
  }

  delay(500);


  //envia as strings pro serial
  Serial.println("temperatura: ");
  Serial.println(strTemperature);
  Serial.println();
  Serial.println("humidade: ");
  Serial.println(strHumidity);
  Serial.println();

  delay(500);
}



void publicar_mqtt(void){
    // Formata as strings a serem enviadas para o dashboard (campos texto)
    sprintf(strTemperature, "%.2f", getTemperature());
    sprintf(strHumidity, "%.2f", getHumidity());

    // Envia as strings ao dashboard MQTT
    MQTT.publish(TOPIC_PUBLISH_TEMPERATURE, strTemperature);
    MQTT.publish(TOPIC_PUBLISH_HUMIDITY, strHumidity);
}






void setup()
{
  //inicializando monitor serial
  Serial.begin(9600);

  // Configura o pino do LED como output e inicializa em nível baixo
  pinMode(RELAY_FECHAR, OUTPUT);
  digitalWrite(RELAY_FECHAR, LOW);
  pinMode(RELAY_ABRIR, OUTPUT);
  digitalWrite(RELAY_ABRIR, LOW);
  pinMode(FIM_DE_CURSO_FECHAR, INPUT);
  pinMode(FIM_DE_CURSO_ABRIR, INPUT);

  // inicializacao do sensor de temperatura
  dht.begin();

  //reset do emperramento
  emperrado = 0;
}



void loop(){
  connect_wifi();
  connect_mqtt();
  getHumidity();
  getTemperature();
  controlador_da_janela();
  publicar_mqtt();
  monitor_serial();

}
