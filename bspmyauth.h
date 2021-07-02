#include "main.h"

#define ssid "SSID"
#define password "PASSWORT"
#if USE_ESP8266
#define esp_restart() []{Serial.println("SHUTDOWN!");}
#endif