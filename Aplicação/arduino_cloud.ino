/*
    Adicione o thingProperties e arduino_secrets no projeto

    Bibliotecas:
    ArduinoIoTCloud Arduino
    Arduino_ConnectionHandler Arduino
        Arduino_DebugUtils
        ArduinoMqttClient
*/

// thingproperties
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"

const char DEVICE_LOGIN_NAME[]  = "COPIE_SEU_DEVICE_ID_AQUI";

const char SSID[]               = SECRET_SSID;
const char PASS[]               = SECRET_OPTIONAL_PASS;
const char DEVICE_KEY[]         = SECRET_DEVICE_KEY;

void onPotChange();

void initProperties(){

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

// Arduino secrets
#define SECRET_SSID "Wokwi-GUEST"
#define SECRET_OPTIONAL_PASS ""
#define SECRET_DEVICE_KEY "S1S5Zc?UN7QtKCI07Bl3laGwl"