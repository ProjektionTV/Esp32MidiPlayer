#ifndef MAINH
#define MAINH

#include "settings.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include <MIDI.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include "types.h"
#include "core/projektionMidi.hpp"
#include "interface.h"
#include "defaults.h"

void mqttReconnect();
void wifiConnect();
void setup();
void loop();

extern notesBufferEntry notesBuffer[NOTES_BUFFER_LENGTH];

extern projektionMidi::projektionMidi *projektionMidiPlayer;
extern midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> MIDI;
extern WiFiClient wiFiClient;
extern PubSubClient psClient;

#endif
