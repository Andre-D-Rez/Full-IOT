#include <WiFi.h>         
#include <PubSubClient.h> // PubSubClient Nick O'Leary

// --- 1. Configurações de Rede e Broker ---
const char* ssid = "REDE"; 
const char* password = "SENHA";
const char* mqtt_server = "broker.hivemq.com"; 
const int mqtt_port = 1883;

// --- 2. Tópicos MQTT ---
const char* topic_comando = "andre/esp32/componente/comando"; // Para RECEBER (Subscribe)
const char* topic_status = "andre/esp32/componente/status";   // Para ENVIAR (Publish) geral

// Tópicos específicos para envio de dados
const char* topic_status_led = "dispositivo/led/status"; 
const char* topic_status_sonic = "dispositivo/sonic/status"; 
const char* topic_status_dht = "dispositivo/dht/status"; 
const char* topic_status_pot = "dispositivo/pot/status"; 
const char* topic_status_ldr = "dispositivo/ldr/status"; 
const char* topic_comando_button = "dispositivo/button/command"; 

// --- 3. Objetos de Conexão ---
WiFiClient espClient;
PubSubClient client(espClient);

// --- 4. Função para conectar no WiFi (Necessário para o MQTT) ---
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

// --- 5. Callback: O que acontece quando CHEGA uma mensagem ---
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

  // Verifica o tópico e o comando (Altere os IFs da mensagem de acordo com as mensagens escritas no payload do MQTT)
  if (String(topic) == topic_comando) {
    if (message == "LIGAR") {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED LIGADO!");
      client.publish(topic_status, "LED LIGADO"); // Publica o status
    } else if (message == "DESLIGAR") {
      digitalWrite(ledPin, LOW);
      Serial.println("LED DESLIGADO!");
      client.publish(topic_status, "LED DESLIGADO"); // Publica o status
    } else {
      Serial.println("Comando inválido. Use LIGAR ou DESLIGAR.");
    }
  }
}

// --- 6. Reconnect: Garante a conexão e se INSCREVE nos tópicos ---
void reconnect() {
  // Loop até que estejamos conectados
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Tenta se conectar com um ID de cliente único
    String clientId = "ESP32ClientAndre";
    
    // Tenta conectar
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      // Uma vez conectado, inscreva-se no tópico de comando
      client.subscribe(topic_comando);
      Serial.print("Inscrito no tópico: ");
      Serial.println(topic_comando);
      
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

// --- 7. Setup do MQTT ---
void setup() {
  Serial.begin(115200);
  
  setup_wifi(); // 1. Conecta WiFi
  
  client.setServer(mqtt_server, mqtt_port); // 2. Define o Broker
  client.setCallback(callback); // 3. Define a função que lê mensagens
}

// --- 8. Loop do MQTT ---
void loop() {
  // Verifica se caiu a conexão e reconecta
  if (!client.connected()) {
    reconnect();
  }
  
  // Mantém a comunicação viva (Keep Alive) e processa callbacks
  client.loop(); 

  // --- 9. Exemplo de PUBLICAÇÃO (Envio de dados) ---
  // (Isso estava dentro da sua função publishSensorData)
  
  // Exemplo: Enviando Ultrassom
  String distanceStr = "100"; // Exemplo, viria da variável distanceCm
  client.publish(topic_status_sonic, distanceStr.c_str());

  // Exemplo: Enviando Temperatura
  String tempStr = "25.5"; // Exemplo, viria do DHT
  client.publish(topic_status_dht, tempStr.c_str());
  
  // Exemplo: Enviando Status do LED
  client.publish(topic_status_led, "ON");

  delay(5000); // Apenas para não flodar o exemplo
}