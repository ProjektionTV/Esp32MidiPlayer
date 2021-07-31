#ifndef INTERFACEH
#define INTERFACEH
#include "main.h"

void setMusicStatus(bool newStatus);
void sendIrcMessage(String s);
void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif
