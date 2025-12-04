#include <DHT.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include "esp_sleep.h"
#include <WiFi.h>
#include <PubSubClient.h>

// --- Configurações de Rede e MQTT ---
const char* ssid = "Wokwi-GUEST"; // Rede padrão do Wokwi
const char* password = "";        // Nenhuma senha
const char* mqtt_server = "broker.hivemq.com"; // Broker MQTT público
const int mqtt_port = 1883;

// --- Tópicos MQTT ---
const char* topic_servo = "meudispositivo/servo/comando"; 
const char* topic_status = "meudispositivo/led/status";   
const char* topic_status2 = "meudispositivo/led/status2";
const char* topic_data = "meudispositivo/dht/data"; 

Servo motor1;

const int greenLed = 16;
const int redLed = 4;
const int pinPot = 35;

#define DHTPIN         32
#define DHTTYPE        DHT22

DHT dht(DHTPIN, DHTTYPE);

// --- Objetos de Conexão ---
WiFiClient espClient;
PubSubClient client(espClient);

// --------------------------------------------------------------------------------

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Função de Callback: Chamada quando uma mensagem MQTT é recebida
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: [");
  Serial.print(topic);
  Serial.print("] ");

  // Converte o payload para String e imprime
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == topic_servo) {
    int angulo = message.toInt();  // converte 0–180
    angulo = constrain(angulo, 0, 180);
    motor1.write(angulo);
    Serial.print("Servo -> ");
    Serial.println(angulo);
  }
}

void reconnect() {
  // Loop até que estejamos conectados
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Tenta se conectar com um ID de cliente único
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect("cliente_servo")) {
      client.subscribe(topic_servo);
    }

    // Tenta conectar
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      // Uma vez conectado, inscreva-se no tópico de comando
    
      // Publica uma mensagem de inicialização
      client.publish(topic_status, "DISPOSITIVO INICIADO");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tente novamente em 5 segundos");
      // Espera 5 segundos antes de tentar novamente
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("Sistema Iniciado: ");
  delay(1000);

  motor1.attach(33);

  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  pinMode(pinPot, INPUT);

  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void publishLedState() {
  String estado = digitalRead(greenLed) ? "LED VERDE LIGADO" : "LED VERDE DESLIGADO";
  client.publish(topic_status, estado.c_str());
  String estado2 = digitalRead(redLed) ? "LED VERMELHO LIGADO" : "LED VERMELHO DESLIGADO";
  client.publish(topic_status2, estado2.c_str());
}

void loop() {
  delay(100);

  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Processa mensagens recebidas e mantém o "ping"

  analogWrite(redLed, LOW);
  analogWrite(greenLed, LOW);

  int pot = analogRead(pinPot);

  int leitura = map(pot, 0, 1023, 0, 180);

  motor1.write(leitura);

  float temperatura = dht.readTemperature();

  if (!isnan(temperatura)) {
    client.publish(topic_data, String(temperatura).c_str());
  }

  if(temperatura < 20 || temperatura > 30)
  {
    analogWrite(redLed, HIGH);
    analogWrite(greenLed, LOW);
    publishLedState();
    motor1.write(0);
    Serial.println("Produção: Em alerta");
    Serial.println("!!!DESLIGUE IMEDIATAMENTE!!!");

    return;
  }else{
    analogWrite(greenLed, HIGH);
    analogWrite(redLed, LOW);
    publishLedState();
    Serial.println("_____________________________");
    Serial.print("Temperatura -> ");
    Serial.println(temperatura);
    Serial.println("_____________________________");
    Serial.println("Produção: Funcionando");
    Serial.println("_____________________________");
    Serial.print("Servo 1 -> ");
    Serial.println(leitura);
    Serial.println("_____________________________");
  }
  
}
