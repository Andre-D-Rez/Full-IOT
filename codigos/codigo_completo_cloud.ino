/* Sketch: Main.ino
  Integrado com Arduino IoT Cloud
*/

#include "thingProperties.h" // Importante: Mantém a conexão com a aba gerada
#include <DHT.h> 

// --- Configurações de Hardware ---
// Input pin
const int buttonPin = 45;
const int pinPot = 15;
const int pinPhot = 1;
const int pino_trigger = 4;
const int pino_echo = 5;

// Output pin
const int buzzerPin = 47;
const int redPin = 17;
const int greenPin = 18;
const int bluePin = 21;

// Variáveis Globais
long duration; 
int distanceCm; 

// Temperatura
#define DHTPIN 10
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Constantes LDR
const float RESISTOR_FIXO = 10000;
const float GAMMA = 0.7;
const float RL10 = 50;

// Interrupção e Debounce
volatile unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;
volatile bool buttonState = false; 

// Interrupção do botão
void IRAM_ATTR handleButtonISR() {
  unsigned long now = millis();
  if ((now - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (digitalRead(buttonPin) == LOW) {
      buttonState = !buttonState;
    }
    lastDebounceTime = now;
  }
}

void setup() {
  // Inicializa Serial e espera portas
  Serial.begin(115200);
  delay(1500); 

  // Configuração definida no thingProperties.h
  initProperties();

  // Conecta ao Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  // Define nível de debug para ver erros de conexão
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Inicializa Hardware
  dht.begin();
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pino_trigger, OUTPUT);
  pinMode(pinPhot, INPUT);
  pinMode(pinPot, INPUT);
  pinMode(pino_echo, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonISR, FALLING);
}

void loop() {
  // Mantém a conexão com a nuvem e sincroniza variáveis
  ArduinoCloud.update();

  noTone(buzzerPin);

  // --- 1. Leitura Ultrassônico ---
  digitalWrite(pino_trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pino_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pino_trigger, LOW);
  duration = pulseIn(pino_echo, HIGH);
  distanceCm = duration / 58;
  
  // Atualiza variável da Nuvem
  sensor_ultrasom = distanceCm;

  // --- 2. Leitura Temperatura ---
  float temperatura = dht.readTemperature();
  bool perigo = (temperatura < 0 || temperatura > 25);
  
  // Atualiza variável da Nuvem (checa se é NaN antes)
  if(!isnan(temperatura)){
    temp = temperatura;
  }

  // --- 3. Leitura LDR ---
  int fotoValue = analogRead(pinPhot);
  float fotoVoltage = fotoValue * 3.3 / 4095.0;
  if (fotoVoltage >= 3.29) fotoVoltage = 3.29;
  float fotoResistance = RESISTOR_FIXO * fotoVoltage / (3.3 - fotoVoltage);
  if (fotoResistance <= 0) fotoResistance = 1;
  float luxCalculado = pow((RL10 * 1000 * pow(10, GAMMA)) / fotoResistance, 1.0 / GAMMA);
  
  bool hasOnlyAmbientLight = (luxCalculado < 2000);
  
  // Atualiza variável da Nuvem
  lux = (int)luxCalculado;

  // --- 4. Leitura Potenciômetro ---
  int leituraPot = analogRead(pinPot);
  pot = leituraPot;

  // --- Lógica de Controle (Cores e Buzzer) ---
  int r = 0, g = 0, b = 0;

  if(buttonState){
      // Sistema "Desativado" pelo botão
      noTone(buzzerPin);
      analogWrite(redPin,   0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin,  0);
      
      // CORRECAO 1: Usar Color()
      led = Color(0, 0, 0); 
      
  } else {
      if(perigo || distanceCm < 100) {
          // SITUAÇÃO DE PERIGO
          tone(buzzerPin, 1000);
          analogWrite(redPin,   0);
          analogWrite(greenPin, 0);
          analogWrite(bluePin,  0);
          
          // CORRECAO 2: Usar Color()
          led = Color(0, 0, 0); 
      } 
      else if(hasOnlyAmbientLight && !perigo) {
          // FUNCIONAMENTO NORMAL
          if (pot < 1365) {
            // Vermelho → Laranja → Amarelo
            r = 255;
            g = map(pot, 0, 1364, 0, 254);
            b = 0;
          } else if (pot < 2730) {
            // Amarelo → Branco
            r = 255;
            g = 255;
            b = map(pot, 1364, 2729, 0, 255);
          } else {
            // Branco → Azul Turquesa → Azul
            r = map(pot, 2730, 4095, 255, 0);
            g = map(pot, 2730, 4095, 255, 0); 
            b = 255;
          }

          analogWrite(redPin,   r);
          analogWrite(greenPin, g);
          analogWrite(bluePin,  b);
          
          // CORRECAO 3: Usar Color() e garantir que o switch está ligado
          led.setSwitch(true); // Garante que a luz aparece como "Ligada" no dashboard
          led = Color(r, g, b);
          
      } else {
          // Luz ambiente muito forte ou condição padrão
          analogWrite(redPin,   0);
          analogWrite(greenPin, 0);
          analogWrite(bluePin,  0);
          
          // CORRECAO 4: Usar Color()
          led = Color(0, 0, 0);
      }
  }
  
  delay(100); 
}

void onPotChange() {
  Serial.println("Mudança no POT via Cloud detectada.");
}