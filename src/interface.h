#ifndef INTERFACEH
#define INTERFACEH

#include <Arduino.h>

void setMusicStatus(bool newStatus);
void sendIrcMessage(String &s);
void sendIrcMessage(const char *s);
void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif
