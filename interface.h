#ifndef INTERFACEH
#define INTERFACEH
#include "main.h"

void setMusicStatus(bool newStatus);
void schreibeChatNachricht(String s);
void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif
