/**
#define SECRET_SSID "Wokwi-GUEST" // Ou sua rede
#define SECRET_OPTIONAL_PASS ""   // Ou sua senha
#define SECRET_DEVICE_KEY "SUA_CHAVE_DO_ARDUINO_CLOUD"
*/

/*
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"

const char DEVICE_LOGIN_NAME[]  = "SEU_DEVICE_ID"; // Pegue no site

const char SSID[]               = SECRET_SSID;
const char PASS[]               = SECRET_OPTIONAL_PASS;
const char DEVICE_KEY[]         = SECRET_DEVICE_KEY;

void onPotChange();

CloudColoredLight led; // Tipo Colorido
float temp;
int lux;
int pot;
int sensor_ultrasom;

void initProperties(){
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  
  ArduinoCloud.addProperty(led, READWRITE, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(temp, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(lux, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(pot, READWRITE, ON_CHANGE, onPotChange);
  ArduinoCloud.addProperty(sensor_ultrasom, READ, ON_CHANGE, NULL);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
*/


/* PROJETO HÍBRIDO V2: Controle Total
   - Arduino IoT Cloud (Dashboard Web)
   - MQTT Panel (App de Celular)
   - Botão Físico e Sensores
*/

#include "thingProperties.h" // Gerencia WiFi e Cloud
#include <PubSubClient.h>    // Gerencia MQTT Manual
#include <DHT.h> 

// --- Configurações MQTT Manual (HiveMQ) ---
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Tópicos MQTT
const char* topic_comando      = "andre/esp32/comando"; // Tópico para RECEBER ordens do celular
const char* topic_status_sonic = "andre/esp32/sonic";
const char* topic_status_dht   = "andre/esp32/dht";
const char* topic_status_pot   = "andre/esp32/pot";

// Cliente WiFi Secundário
WiFiClient espClient;
PubSubClient client(espClient);

// --- Hardware ---
const int buttonPin = 45;
const int pinPot = 15;
const int pinPhot = 1;
const int pino_trigger = 4;
const int pino_echo = 5;
const int buzzerPin = 47;
const int redPin = 17;
const int greenPin = 18;
const int bluePin = 21;

// Sensores
#define DHTPIN 10
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

long duration; 
int distanceCm; 

// Variáveis LDR
const float RESISTOR_FIXO = 10000;
const float GAMMA = 0.7;
const float RL10 = 50;

// Controle de Tempo
unsigned long previousMillis = 0;
const long interval = 2000; 

// Controle Botão e Estado
volatile unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;
volatile bool buttonState = false; // false = SISTEMA LIGADO / true = SISTEMA DESLIGADO

// Interrupção Física
void IRAM_ATTR handleButtonISR() {
  unsigned long now = millis();
  if ((now - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (digitalRead(buttonPin) == LOW) {
      buttonState = !buttonState; // Inverte o estado
    }
    lastDebounceTime = now;
  }
}

// --- FUNÇÃO CALLBACK (Onde a mágica do App acontece) ---
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Comando recebido do App: ");
  Serial.println(message);

  // Lógica de Controle pelo App
  // Vamos fazer o App simular o botão físico (Ativar/Desativar sistema)
  
  if (String(topic) == topic_comando) {
    if (message == "DESLIGAR") {
      buttonState = true; // Desativa o sistema
      Serial.println("App ordenou: DESLIGAR SISTEMA");
    } 
    else if (message == "LIGAR") {
      buttonState = false; // Ativa o sistema
      Serial.println("App ordenou: LIGAR SISTEMA");
    }
  }
}

void reconnectMQTT() {
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    Serial.print("Reconectando MQTT HiveMQ...");
    String clientId = "ESP32_Hybrid_" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      
      // *** IMPORTANTE: Assina o tópico para escutar o celular ***
      client.subscribe(topic_comando);
      Serial.println("Escutando comandos...");
      
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500); 

  // 1. Inicializa Hardware
  dht.begin();
  pinMode(redPin, OUTPUT); pinMode(greenPin, OUTPUT); pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT); pinMode(pino_trigger, OUTPUT);
  pinMode(pinPhot, INPUT); pinMode(pinPot, INPUT); pinMode(pino_echo, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonISR, FALLING);

  // 2. Inicializa Arduino Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // 3. Configura MQTT Manual
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Registra a função que lê as mensagens
}

void loop() {
  ArduinoCloud.update(); 
  
  if (!client.connected()) {
    reconnectMQTT(); 
  }
  client.loop(); // Checa se chegou mensagem do App

  noTone(buzzerPin);

  // --- Leituras de Sensores ---
  digitalWrite(pino_trigger, LOW); delayMicroseconds(2);
  digitalWrite(pino_trigger, HIGH); delayMicroseconds(10);
  digitalWrite(pino_trigger, LOW);
  duration = pulseIn(pino_echo, HIGH);
  distanceCm = duration / 58;
  sensor_ultrasom = distanceCm; 

  float temperatura = dht.readTemperature();
  if(!isnan(temperatura)) temp = temperatura;
  bool perigo = (temperatura < 0 || temperatura > 25);

  int fotoValue = analogRead(pinPhot);
  float fotoVoltage = fotoValue * 3.3 / 4095.0;
  if (fotoVoltage >= 3.29) fotoVoltage = 3.29;
  float fotoResistance = RESISTOR_FIXO * fotoVoltage / (3.3 - fotoVoltage);
  if (fotoResistance <= 0) fotoResistance = 1;
  float luxCalculado = pow((RL10 * 1000 * pow(10, GAMMA)) / fotoResistance, 1.0 / GAMMA);
  lux = (int)luxCalculado;
  bool hasOnlyAmbientLight = (luxCalculado < 2000);

  int leituraPot = analogRead(pinPot);
  pot = leituraPot; 

  // --- Lógica de Controle ---
  int r = 0, g = 0, b = 0;

  // Se buttonState for TRUE, o sistema está DESLIGADO (seja pelo botão ou pelo App)
  if(buttonState){
      apagarLeds();
      led = Color(0, 0, 0); 
  } else {
      // SISTEMA LIGADO
      if(perigo || distanceCm < 100) {
          tone(buzzerPin, 1000);
          apagarLeds();
          led = Color(0, 0, 0); 
      } 
      else if(hasOnlyAmbientLight && !perigo) {
          if (pot < 1365) { r = 255; g = map(pot, 0, 1364, 0, 254); b = 0; } 
          else if (pot < 2730) { r = 255; g = 255; b = map(pot, 1364, 2729, 0, 255); } 
          else { r = map(pot, 2730, 4095, 255, 0); g = map(pot, 2730, 4095, 255, 0); b = 255; }
          
          analogWrite(redPin, r); analogWrite(greenPin, g); analogWrite(bluePin, b);
          led.setSwitch(true);
          led = Color(r, g, b); 
      } else {
          apagarLeds();
          led = Color(0, 0, 0);
      }
  }

  // --- Envio para o App (Dashboards) ---
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (client.connected()) {
      client.publish(topic_status_sonic, String(distanceCm).c_str());
      client.publish(topic_status_dht, String(temp).c_str());
      client.publish(topic_status_pot, String(pot).c_str());
      
      // Publica o status do sistema para o botão do App atualizar
      String statusSistema = buttonState ? "DESLIGADO" : "LIGADO";
      client.publish("andre/esp32/status", statusSistema.c_str());
    }
  }
}

void apagarLeds() {
  analogWrite(redPin, 0); analogWrite(greenPin, 0); analogWrite(bluePin, 0);
}
void onPotChange() {}