const int pino_trigger = 4;
const int pino_echo = 5;

long duration; // Variável para armazenar a duração do pulso
int distanceCm; // Variável para armazenar a distância em cm

void setup() {
  pinMode(pino_trigger, OUTPUT);
  pinMode(pino_echo, INPUT);
}

void loop() {
    // 1. Limpa o pino TRIG para garantir um pulso limpo (LOW)
  digitalWrite(pino_trigger, LOW);
  delayMicroseconds(2); // Pequeno atraso

  // 2. Envia um pulso de 10 microssegundos (HIGH) no pino TRIG
  digitalWrite(pino_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pino_trigger, LOW);

  // 3. Mede a duração do pulso no pino ECHO
  // pulseIn() retorna o tempo em microssegundos que o pino ECHO ficou HIGH
  duration = pulseIn(pino_echo, HIGH);

  // 4. Calcula a distância
  // A distância é calculada pela fórmula: (duração / 2) / 29.1, ou simplesmente
  // distância = duração / 58
  distanceCm = duration / 58;
}