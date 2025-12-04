#include <ESP32Servo.h> // ESP32Servo Kevin Harrington

Servo motor1;

void setup() {
    motor.attach(33);
}

void loop() {
    int leitura = map(pot, 0, 1023, 0, 180);

    motor1.write(leitura);
}