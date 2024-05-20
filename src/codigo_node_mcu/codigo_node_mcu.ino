#include <ESP8266_ISR_Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//WiFi
const char* SSID = "";                                                    // Nome da rede WiFi
const char* PASSWORD = "";                                                // Senha da rede WiFi
WiFiClient wifiClient;                                                    // Objeto WifI
void recebePacote(char* topic, byte* payload, unsigned int length);       // funcao que recebe informacoes via MQTT


//MQTT Server
const char* BROKER_MQTT = "test.mosquitto.org";                           // URL do broker MQTT
int BROKER_PORT = 1883;                                                   // Porta do Broker MQTT

#define ID_MQTT "MEU_ESP_NTN"                                             //ID DO NODEMCU
#define TOPIC_PUBLISH "TESTE_COM_NTN"                                     //TOPICO COM INFORMACAO DE REMEDIO NO COMPARTIMENTO
PubSubClient MQTT(wifiClient);                                            // Instancia o Cliente MQTT passando o objeto espClient

// Config do servidor NTP
WiFiUDP ntpUDP;                                                         // Objeto WifI UDP
NTPClient timeClient(ntpUDP, "pool.ntp.org");                           // Objeto NTPClient


/* ----------------- Variáveis sensor de cores ----------------- */
const int PINO_SENSOR_S0 = D1;                                          // Pino 8 da BlackBoard conectado ao pino S0 do TCS230
const int PINO_SENSOR_S1 = D2;                                          // Pino 9 da BlackBoard conectado ao pino S1 do TCS230
const int PINO_SENSOR_S2 = D3;                                          // Pino 12 da BlackBoard conectado ao pino S2 do TCS230
const int PINO_SENSOR_S3 = D4;                                          // Pino 11 da BlackBoard conectado ao pino S3 do TCS230
const int PINO_SENSOR_OUT = D5;                                         // Pino 10 da BlackBoard conectado ao pino OUT do TCS230

int red;                                                                //Declaração da variável que representará a cor vermelha
int green;                                                              //Declaração da variável que representará a cor verde
int blue;                                                               // Declaração da variável que representará a cor azul
/* ----------------- Variáveis sensor de cores ----------------- */


int servoIndex1 = -1;                                                   // Variável place holder para utilização das funções servo ESP


void setup() {

  /* ----------------- CONFIGURA FUSO HORÁRIO DE ACORDO COM DOC DA BIBLIOTECA NTP -----------------*/  
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(-10800); // BRASILIA = GMT -3
  /* ----------------- CONFIGURA FUSO HORÁRIO DE ACORDO COM DOC DA BIBLIOTECA NTP -----------------*/  


  /* ----------------- CONFIGURACAO SENSOR DE CORES -----------------*/  
  pinMode(PINO_SENSOR_S0, OUTPUT);                                    // Pino S0 configurado como saida
  pinMode(PINO_SENSOR_S1, OUTPUT);                                    // Pino S1 configurado como saida
  pinMode(PINO_SENSOR_S2, OUTPUT);                                    // Pino S2 configurado como saida
  pinMode(PINO_SENSOR_S3, OUTPUT);                                    // Pino S3 configurado como saida
  pinMode(PINO_SENSOR_OUT, INPUT);                                    // Pino OUT configurado como entrada

  digitalWrite(PINO_SENSOR_S0,HIGH);                                  // Inicia o codigo com o pino S0 em nivel alto
  digitalWrite(PINO_SENSOR_S1,LOW);                                   // Inicia o codigo com o pino S1 em nivel baixo
  
  
  /* ----------------- CONFIGURACAO SENSOR DE CORES -----------------*/  


  /* ----------------- CONFIGURACAO Servo -----------------*/  
  servoIndex1 = ISR_Servo.setupServo(D6, 800, 2450);
  ISR_Servo.setPosition(servoIndex1, 130);                          // Coloca servo na posicao inicial (130) = porta fechada
  /* ----------------- CONFIGURACAO Servo -----------------*/  


  Serial.begin(115200);                                             // inicia comunicação Serial

  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(recebePacote);
  
}
void loop() {
  
  timeClient.update();                                        // atualiza horario
  String formattedTime = timeClient.getFormattedTime();       // salva horario formatado
  
  // Se horario especificado, porta se abre: Servo em posicao 180
  if(formattedTime == "22:08:00"){
    ISR_Servo.setPosition(servoIndex1, 180);
    delay(1000);
  }

  // Apos tempo determinado, a porta se fecha novamente
  if(formattedTime == "22:09:00"){
    ISR_Servo.setPosition(servoIndex1, 130);
    delay(1000);
  }
  
  // Conexão broker MQTT
  if (MQTT.connect(ID_MQTT)) {
      //Serial.println("Conectado ao Broker com sucesso!");
      MQTT.subscribe(TOPIC_PUBLISH);
  } else {
    // Serial.println("Noo foi possivel se conectar ao broker.");
    // Serial.println("Nova tentatica de conexao em 2s");
    // Serial.println(WiFi.status());
    delay(2000);
  }

  // Configura o TCS230 para ler o canal Verde
  digitalWrite(PINO_SENSOR_S2, HIGH); 
  digitalWrite(PINO_SENSOR_S3, HIGH); 

  // Le o canal verde do sensor
  green = pulseIn(PINO_SENSOR_OUT, LOW); 

  // Printa o valor do canal verde
  Serial.print(" G: ");
  Serial.println(green); 
  delay(15); 


  // Sensor calibrado para limiar de 520, menos = não há comprimido, mais = há comprimidos
  if(green < 520){
    enviaMessage("Sim");
  }
  else{
    enviaMessage("Não");
  }
  delay(1000);

  
  MQTT.loop();
}


/*Funcao Boiler-Plate que recebe informações via MQTT*/
void recebePacote(char* topic, byte* payload, unsigned int length){

  String msg;
 
  for(int i = 0; i < length; i++){
    char c = (char)payload[i];
    msg += c;
  }

  if (msg =="0"){
      Serial.println("desliga");
      digitalWrite(LED_BUILTIN, HIGH);
  }
  if (msg == "1"){
      Serial.println("liga");
      digitalWrite(LED_BUILTIN, LOW);
  }

  Serial.println(msg);

}


/*Funcao Boiler-Plate que publica informações via MQTT*/
void enviaMessage(const char* msg) {
  if (MQTT.connected()) {
    MQTT.publish(TOPIC_PUBLISH, msg);
    Serial.println("Published message: " + String(msg));
  } else {
    Serial.println("MQTT not connected, unable to publish message");
  }
}



