const int pinPhot = 1;

//constantes para leitura do sensor de iluminação
const float RESISTOR_FIXO = 10000;
const float GAMMA = 0.7;
const float RL10 = 50;

void setup() {
    pinMode(pinPhot, INPUT);
}

void loop(){
    int fotoValue = analogRead(pinPhot); // Fotorresistor
    float fotoVoltage = fotoValue * 3.3 / 4095.0;
    if (fotoVoltage >= 3.29) fotoVoltage = 3.29;
    float fotoResistance = RESISTOR_FIXO * fotoVoltage / (3.3 - fotoVoltage); //divisor de tensao
    if (fotoResistance <= 0) fotoResistance = 1;
    float lux = pow((RL10 * 1000 * pow(10, GAMMA)) / fotoResistance, 1.0 / GAMMA);
}