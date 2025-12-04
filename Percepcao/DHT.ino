#include <DHT.h> // DHT sensor adafruit

#define DHTPIN         10
#define DHTTYPE        DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    dht.begin();
}

void loop() {
    float temperatura = dht.readTemperature();
}