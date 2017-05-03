#include <ESP8266WiFi.h> //bilbioteca para usar funções wifi do esp
#include <PubSubClient.h> //cliente MQTT

/**************Definição da rede Wifi*****************************************/
const char* SSID = "Relicario"; //nome da rede
const char* PASSWORD = "21011993"; //senha

/************ *Definição do servidor Broker***********************************/
const char* BROKER_MQTT = "192.168.1.7"; //endereço do broker
int BROKER_PORT = 1883; //porta de comunicação do mqtt
/**************Definicação do Cliente wifi e do PubSubClient******************/
WiFiClient espClient;           //cria objeto cliente
PubSubClient MQTT(espClient);   //instancia o cliente mqtt passando o espClient
/******************Definição dos pinos do projeto*****************************/
//define o pino do led
#define pinLed D0
//sensor de umidade
#define SensorUmi 0
//Porta D1 do nodemcu GPIO5
#define WaterFlowSensor 5
//Define pino do rele
#define rele 4 // Porta D2 do nodemcu GPIO4
#define top_umidade "Umidade"
#define top_vazao "vazao"
int Turbine;
/****************Funções a serem usadas(Prototypes)***************************/
void initPins();
void initSerial();
void initWiFi();
void initMQTT();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
float ReadUmidade(void);
void ControlRele(void);
void rpm(void);
/***************Inicialização do projeto**************************************/
void setup() {
  initPins();
  initSerial();
  initWiFi();
  initMQTT();
  ReadUmidade();
}
/***************Função inicializa pinos**************************************/
void initPins() {
  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, 0);
}
/**************Inicicializa Monitor Serial a baud de 115200******************/
void initSerial() {
  Serial.begin(9600);
}
/**************Inicicializa Conexao Wifi*************************************/
void initWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se em: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD); //conecta-se a rede usando o SSID e PASSWORD
  //Aguarda conexão...
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado na Rede ");
  Serial.print(SSID);
  Serial.println(" | IP ");
  Serial.println(WiFi.localIP());
}
//conecta ao broker (funcao padrao)
void initMQTT(){
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);     //seta parametros de conexão MQTT
  MQTT.setCallback(mqtt_callback);              //atribui função callback
}
//função callback, chamada sempre que qualquer informação de um dos tópicos chega
//Funcao padrao do sistema PubSubClient.h
void mqtt_callback(char* topic, byte* payload, unsigned int length) {

  String message;
  //obtem a string do payload recebido
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    message += c;
  }
  //printa informações
  Serial.print("Tópico ");
  Serial.print(topic);
  Serial.print(" | ");
  Serial.println(message);
  //realiza ação de acordo com a string recebida
    if (message == "1") {
      digitalWrite(16, 1); //liga o led
    } else {
      digitalWrite(16, 0);//desliga o led
    }
  //"zera" string e espera transmissão de dados de saída para completar (?)
  message = "";
  Serial.println();
  Serial.flush();
}
//reconecta ao broker caso a conexão caia ou não esteja conectado
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    //se conseguir conexão o subscribe do(s) topico(s) é feito
    if (MQTT.connect("ESP8266-ESP12-E")) {
      Serial.println("Conectado");
      MQTT.subscribe("led");
    } else { //caso não consiga conectar espera 2 segundos para tentar de novo
      Serial.println("Falha ao Reconectar");
      Serial.println("Tentando se reconectar em 2 segundos");
      delay(2000);
    }
  }
}
float ReadUmidade(void)
{
    int UmidadePercentual;
    int ValorADC;
    ValorADC = analogRead(0);
    Serial.print("[Leitura ADC] ");
    Serial.println(ValorADC);
    //Umidade a 100% ==0
     UmidadePercentual = 100 * ((418-(float)ValorADC) / 418);
     Serial.print("[Umidade Percentual] ");
     Serial.print(UmidadePercentual);
     Serial.println("%");
     delay(1000);
     return UmidadePercentual;
}
void rpm () 	 //Function interrupt call
{
Turbine++; //Function measures the incrementing falling edge
}
void ControlRele(void){
  if (ReadUmidade()<=50)
  {
    digitalWrite(rele, 1);
  }
  else
  {
    digitalWrite(rele, 0);
  }
}
void SendMQTT(){
  int Calc;
  float UmidadeL;
  int UmidadeT;
  char FieldUmidade[12];
  UmidadeL = ReadUmidade();
  UmidadeT = int(UmidadeL);
  Calc = (Turbine * 60 / 7.5); //PULSE multiply by 60 divided by 7.5Q, equal FLOW RATE in L / hour
  Serial.print(Calc, DEC); //Show the number calculated above to communication port
  Serial.print(" L/hour\r\n"); //Prints L / hour & returns a new li
  MQTT.publish(top_umidade, String(UmidadeT).c_str());
  MQTT.publish(top_vazao, String(Calc).c_str());
}
//reconecta wifi
void recconectWiFi() {
  //enquando o status for diferente, fica tentando conecctar
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
}
void loop() {
  if (!MQTT.connected()) {//se a conexão cair
    reconnectMQTT();       //tenta conectar novamente
  }
  recconectWiFi(); // se a conexão do esp cair, tenta conectar de novo
  MQTT.loop(); //keep-alive de comunicação com o broker MQTT
  SendMQTT();
//  Serial.println(FieldUmidade);
}
