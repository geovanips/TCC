/***********************************************************/
/*Alpha_02 Automacao de irrigacao TCC 1 10/04/2017         */
/*Teste de publicacao no thingspeak com o sensor de umidade*/
/*Aluno:Geovani Pereira da Silva                           */
/* http://blog.filipeflop.com/wireless/planta-iot-com-     */
/* esp8266-nodemcu.html                                    */
/***********************************************************/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//Rede Wifi e Senha
#define SSID_REDE     ""
#define SENHA_REDE    ""
//Define o intervalo de envio do thingspeak em milisegundos
#define INTERVALO_ENVIO_THINGSPEAK  30000
//Define pino de leitura do sensor de fluxo
#define WaterFlowSensor 5
//Define pino do rele
#define rele 4
//constantes e variáveis globais
char EnderecoAPIThingSpeak[] = "api.thingspeak.com";
String ChaveEscritaThingSpeak = "D7QFC5Z4B1QEF4TX";
long lastConnectionTime;
WiFiClient client;
float UmidadePercentual;

//prototypes
void EnviaInformacoesThingspeak(String StringDados);
void FazConexaoWiFi(void);
float FazLeituraUmidade(void);
void StopConThingspeak(void);
//Criando o servidor web na porta 80
ESP8266WebServer server(80);
void PaginaWeb(void);
void ControlRele(void);
//float FazLeituraFluxo(void);
/*
 * Implementações
 */

//Função: envia informações ao ThingSpeak
//Parâmetros: String com a  informação a ser enviada
//Retorno: nenhum
void EnviaInformacoesThingspeak(String StringDados)
{
    if (client.connect(EnderecoAPIThingSpeak, 80))
    {
        //faz a requisição HTTP ao ThingSpeak
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+ChaveEscritaThingSpeak+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(StringDados.length());
        client.print("\n\n");
        client.print(StringDados);

        lastConnectionTime = millis();
        Serial.println("- Informações enviadas ao ThingSpeak!");
     }
}

//Desconecta do ThingSpeak apos envio
void StopConThingspeak(void){
  if (client.connected())
  {
      client.stop();
      Serial.println("- Desconectado do ThingSpeak");
      Serial.println();
  }
}
void ControlRele(void){
  if (UmidadePercentual<=50)
  {
    digitalWrite(rele, 1);
  }
  else
  {
    digitalWrite(rele, 0);
  }
}
//Pagina principal HTML
void PaginaWeb(void)
{
  String recebe=String(UmidadePercentual, DEC);
  // HTML da pagina principal
  String html ="<!DOCTYPE HTML>\r\n<html>\r\n";
  html += "<h3>ESP8266 Servidor Web - TCC</h3>";
//  html += "<p>LED <a href=\"?function=rele1_on\"><button>LIGA</button></a><a href=\"?function=rele1_off\"><button>DESLIGA</button></a></p>";
  html += "<head><title>Automacao de irrigacao</title></head>";
  html += "<body>";
  html += "<p>Umidade é:</p>";
  html += String(UmidadePercentual);
  html += "</body>";
  html += "<h4>Irrigacao automatica - Geovani</h4>";
  html += "</html>\n";
  // Enviando HTML para o servidor
  server.send(200, "text/html", html);
}
//Função: faz a conexão WiFI
//Parâmetros: nenhum
//Retorno: nenhum
void FazConexaoWiFi(void)
{
    client.stop();
    Serial.println("Conectando-se à rede WiFi...");
    Serial.println();
    delay(1000);
    WiFi.begin(SSID_REDE, SENHA_REDE);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connectado com sucesso!");
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());

    delay(1000);
}

//Função: faz a leitura do nível de umidade
//Parâmetros: nenhum
//Retorno: umidade percentual (0-100)
//Observação: o ADC do NodeMCU permite até, no máximo, 1V. Dessa forma,
//            para 1V, obtem-se (empiricamente) 418 como leitura de ADC
float FazLeituraUmidade(void)
{
    int ValorADC;
    ValorADC = analogRead(0);   //418 -> 1.0V
    Serial.print("[Leitura ADC] ");
    Serial.println(ValorADC);

     //Quanto maior o numero lido do ADC, menor a umidade.
     //Sendo assim, calcula-se a porcentagem de umidade por:
     //
     //   Valor lido                 Umidade percentual
     //      _    0                           _ 100
     //      |                                |
     //      |                                |
     //      -   ValorADC                     - UmidadePercentual
     //      |                                |
     //      |                                |
     //     _|_  418                         _|_ 0
     //
     //   (UmidadePercentual-0) / (100-0)  =  (ValorADC - 418) / (-418)
     //      Logo:
     //      UmidadePercentual = 100 * ((418-ValorADC) / 418)

     UmidadePercentual = 100 * ((418-(float)ValorADC) / 418);
     Serial.print("[Umidade Percentual] ");
     Serial.print(UmidadePercentual);
     Serial.println("%");

     return UmidadePercentual;
}
void GeraLeitura(void){
  float UmidadePercentualLida;
  int UmidadePercentualTruncada;
  char FieldUmidade[11];
  //Força desconexão ao ThingSpeak (se ainda estiver desconectado)
  StopConThingspeak();
  UmidadePercentualLida = FazLeituraUmidade();
  UmidadePercentualTruncada = (int)UmidadePercentualLida; //trunca umidade como número inteiro
  //verifica se está conectado no WiFi e se é o momento de enviar dados ao ThingSpeak
  if(!client.connected() &&
    (millis() - lastConnectionTime > INTERVALO_ENVIO_THINGSPEAK))
  {
      sprintf(FieldUmidade,"field1=%d",UmidadePercentualTruncada);
      sprintf(FieldWaterSensor,"field2=%d",UmidadePercentualTruncada);
      String postStr = ChaveEscritaThingSpeak;
             postStr += "&amp;";
             postStr += FieldUmidade;
             postStr += "&amp;";
             postStr += FieldWaterSensor;
      EnviaInformacoesThingspeak(postStr);
      Serial.println(postStr);
  }
}

void setup()
{
    Serial.begin(9600);
    lastConnectionTime = 0;
    FazConexaoWiFi();
    //Estados das portas
    pinMode(rele, OUTPUT);
    digitalWrite(rele, LOW);
    pinMode(WaterFlowSensor, INPUT);
    digitalWrite(WaterFlowSensor, HIGH);
    // Atribuindo urls para funções
    server.on("/", PaginaWeb);
    //inicia o server web
    server.begin();
    //Sinalizando http server iniciou
    Serial.println("HTTP server started");
}

//loop principal
void loop()
{
     GeraLeitura();
     server.handleClient();
     delay(1000);
}
