#ifndef MAINH
#define MAINH

#include <PubSubClient.h>
#include "core/projektionMidi.hpp"

void mqttReconnect();
void setup();
void loop();

extern projektionMidi::projektionMidi *projektionMidiPlayer;
extern PubSubClient psClient;

#endif
