const int buzzerPin = 47;

void setup() {
    pinMode(buzzerPin, OUTPUT);
}

void loop() {
    noTone(buzzerPin);

    tone(buzzerPin, 1000);
}