const int pinPot = 15;

void setup() {
    pinMode(pinPot, INPUT);
}

void loop() {
    int potValue = analogRead(pinPot);
}