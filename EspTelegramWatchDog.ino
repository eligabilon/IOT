//Programa: Bot Telegram com ESP8266 NodeMCU
//Eli Gabilon 08-2019
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include "CTBot.h"
CTBot myBot;
#include <Ticker.h>

//Definicoes da rede wifi e conexao
String ssid  = "NOME_REDE"; //Nome da sua rede wifi
String pass  = "SENHA"; //Senha da sua rede wifi
String token = "TOKEN"; //Token bot Telegram

//Pinos do led e bombaAgua
uint8_t bombaAgua = D6;
uint8_t ledPiscina = D3;
uint8_t ledConectInternet = D4;

Ticker secondTick;

volatile int watchdogCount = 0;

void ISRwatchdog() {
  watchdogCount++;
  TBMessage msg;
  if (watchdogCount == 15) {
    Serial.println();
    Serial.println("Travou... resetei!!!");
    ESP.reset();
    myBot.sendMessage(msg.sender.id, "Travei, precisei reiniciar.");
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Inicializando bot com Telegram...");

  //Define os pinos dos leds como saida e apaga os leds
  pinMode(bombaAgua, OUTPUT);
  pinMode(ledPiscina, OUTPUT);
  pinMode(ledConectInternet, OUTPUT);
  digitalWrite(bombaAgua, LOW);
  digitalWrite(ledPiscina, LOW);
  digitalWrite(ledConectInternet, HIGH);
  delay(2000);

  //Conexao na rede wifi
  myBot.wifiConnect(ssid, pass);

  //Define o token
  myBot.setTelegramToken(token);

  verificaConexao();
  secondTick.attach(1, ISRwatchdog);
}

void verificaConexao() {
  //Verifica a conexao
  if (myBot.testConnection())
  {
    digitalWrite(ledConectInternet, LOW);
    Serial.println("Conexao Ok!");
  }
  else
  {
    digitalWrite(ledConectInternet, HIGH);
    delay(2000);
    myBot.wifiConnect(ssid, pass);
    Serial.println("Falha na conexao!");
  }
}

void loop()
{
  verificaConexao();

  Serial.printf("WatchDog = %d \n", watchdogCount);

  watchdogCount = 0;

  //Variavel que armazena a mensagem recebida
  TBMessage msg;

  //Verifica se chegou alguma mensagem
  if (myBot.getNewMessage(msg))
  {
    if (msg.text.equalsIgnoreCase("/start"))
    {
      myBot.sendMessage(msg.sender.id, "Começamos agora...");
    }
    else if (msg.text.equalsIgnoreCase("COMANDOS"))
    {
      myBot.sendMessage(msg.sender.id, "COMANDOS PARA CONTROLAR A BOMBA E O LED DA PISCINA \n\n LIGAR BOMBA - para ligar a bomba da piscina \n\n DESLIGAR BOMBA - para desligar a bomba da piscina \n\n LIGAR LED - para ligar o led da piscina \n\n DESLIGAR LED - para desligar o led da piscina");
    }
    else if (msg.text.equalsIgnoreCase("EAE") || msg.text.equalsIgnoreCase("OI") || msg.text.equalsIgnoreCase("OLA"))
    {
      myBot.sendMessage(msg.sender.id, "Ae blz");
    }
    else if (msg.text.equalsIgnoreCase("LIGA TUDO") || msg.text.equalsIgnoreCase("LIGAR TUDO"))
    {
      digitalWrite(ledPiscina, HIGH);
      delay(50);
      digitalWrite(bombaAgua, HIGH);
      myBot.sendMessage(msg.sender.id, "Led e Bomba da piscina LIGADOS!");
    }
    //Verifica se foi recebia a mensagem "Liga bomba"
    else if (msg.text.equalsIgnoreCase("LIGA BOMBA") || msg.text.equalsIgnoreCase("LIGAR BOMBA"))
    {
      digitalWrite(bombaAgua, HIGH);
      //Retorna uma pensagem informando que a bomba foi ligada
      myBot.sendMessage(msg.sender.id, "Bomba da piscina LIGADA!");
      delay(10);
      myBot.sendMessage(msg.sender.id, "Mais alguma coisa?");
    }
    else if (msg.text.equalsIgnoreCase("N") || msg.text.equalsIgnoreCase("NÃO") || msg.text.equalsIgnoreCase("Ñ"))
    {
      myBot.sendMessage(msg.sender.id, "então blz");
    }
    else if (msg.text.equalsIgnoreCase("S") || msg.text.equalsIgnoreCase("SIM"))
    {
      myBot.sendMessage(msg.sender.id, "pode dizer");
    }
    else if (msg.text.equalsIgnoreCase("DESLIGA BOMBA") || msg.text.equalsIgnoreCase("DESLIGAR BOMBA"))
    {
      digitalWrite(bombaAgua, LOW);
      myBot.sendMessage(msg.sender.id, "Bomba da piscina DESLIGADA!");
    }
    else if (msg.text.equalsIgnoreCase("LIGA LED") || msg.text.equalsIgnoreCase("LIGAR LED"))
    {
      digitalWrite(ledPiscina, HIGH);
      myBot.sendMessage(msg.sender.id, "Led da piscina LIGADO!");
    }
    else if (msg.text.equalsIgnoreCase("DESLIGA LED") || msg.text.equalsIgnoreCase("DESLIGAR LED"))
    {
      digitalWrite(ledPiscina, LOW);
      myBot.sendMessage(msg.sender.id, "Led da piscina DESLIGADO!");
    }
    else if (msg.text.equalsIgnoreCase("DESLIGA TUDO") || msg.text.equalsIgnoreCase("DESLIGAR TUDO"))
    {
      digitalWrite(ledPiscina, LOW);
      delay(50);
      digitalWrite(bombaAgua, LOW);
      myBot.sendMessage(msg.sender.id, "Led e Bomba da piscina DESLIGADOS!");
    }
    else if (msg.text.equalsIgnoreCase("DESLIGA") || msg.text.equalsIgnoreCase("DESLIGAR"))
    {
      myBot.sendMessage(msg.sender.id, "O q quer q eu desligue, o LED da piscina ou a BOMBA? \n\n se for o LED digita DESLIGA LED \n\n e se for o BOMBA digita DESLIGA BOMBA \n\n ou para desligar tudo digita DESLIGA TUDO");
      delay(10);
    }
    else
    {
      //Caso receba qualquer outro comando
      String reply;
      reply = (String)"Ixi " + msg.sender.username + (String)"! essa palavra não é um comando válido p mim. Digite a palavra COMANDOS para ver o q eu aceito.";
      myBot.sendMessage(msg.sender.id, reply);
    }
  }
  delay(1000);
}
