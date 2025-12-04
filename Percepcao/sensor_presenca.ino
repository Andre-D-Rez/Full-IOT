const int pinPIR = 13;

bool movimentoDetectado = false;   // Variável para armazenar estado do PIR

void setup() {
    pinMode(pinPIR, INPUT);
}

void loop() {
    // --- 4. LEITURA DO PIR (Movimento) ---
    // O PIR retorna HIGH (1) se houver movimento, LOW (0) se não houver
    int leituraPIR = digitalRead(pinPIR);
    
    if (leituraPIR == HIGH) {
        if (!movimentoDetectado) { // Entrou aqui só na borda de subida (primeira vez)
        Serial.println("!!! MOVIMENTO DETECTADO !!!");
        movimentoDetectado = true;
        }
    } else {
        if (movimentoDetectado) {
        Serial.println("... movimento parou ...");
        movimentoDetectado = false;
        }
    }
}