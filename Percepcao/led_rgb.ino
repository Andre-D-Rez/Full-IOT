const int redPin = 17;
const int greenPin = 18;
const int bluePin = 21;

void setup() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
}

void loop(){
    analogWrite(redPin,   0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin,  0);

    // potenciometro
    int r = 0, g = 0, b = 0;
    r = 255;
    g = map(potValue, 0, 1364, 0, 254); //165
    b = 0;

    // ultrassonico
    r = 255;
    g = map(distanceCm, 0, 133, 0, 254); //165
    b = 0;
}