// Botao de ligar e desligar sistema
const int buttonPin = 45;

// Interrupção e Debounce
// ----- VARIÁVEIS DE INTERRUPÇÃO E DEBOUNCE -----
volatile unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;
volatile bool buttonState = false;
volatile int bounceCount = 0;

void IRAM_ATTR handleButtonISR() {
  unsigned long now = millis();
  if ((now - lastDebounceTime) > DEBOUNCE_DELAY) {
    // le a borda
    if (digitalRead(buttonPin) == LOW) {
      // alterna estado lógico (isso apenas atualiza flag; o envio ao Cloud acontece no loop)
      buttonState = !buttonState;
    }
    bounceCount++;
    lastDebounceTime = now;
  }
}

void setup() {
    pinMode(buttonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonISR, FALLING);
}

void loop() {
    if(buttonState){
        noTone(buzzerPin);
        Serial.println("Sistema desativado!!! ");

        analogWrite(redPin,   0);
        analogWrite(greenPin, 0);
        analogWrite(bluePin,  0);
    } else {
}

#-----------------------------------------------------------------------------#

// Variável para rastrear o estado atual do sistema
// true = LED 1 Aceso / LED 2 Apagado
// false = LED 1 Apagado / LED 2 Aceso
bool estadoAtualLED = true; 

void loop() {
    int estadoAtualBotao = digitalRead(botaoPin);

    // 2. Detecção de Borda de Descida (Button Press)
    // O botão foi pressionado (passou de HIGH para LOW)
    if (estadoAnteriorBotao == HIGH && estadoAtualBotao == LOW) {
        
        // 3. Inverte o estado do sistema
        estadoAtualLED = !estadoAtualLED; // Inverte true para false, ou false para true
        
        // 4. Aplica o novo estado aos LEDs
        if (estadoAtualLED) {
            digitalWrite(blueLed, HIGH);  // LED 1 Aceso
            digitalWrite(redLed, LOW);   // LED 2 Apagado
        } else {
            digitalWrite(blueLed, LOW);   // LED 1 Apagado
            digitalWrite(redLed, HIGH);  // LED 2 Aceso
        }
        
        // Pequeno atraso para "debounce" e evitar múltiplas leituras de um único clique
        delay(50); 
    }

    estadoAnteriorBotao = estadoAtualBotao;
}