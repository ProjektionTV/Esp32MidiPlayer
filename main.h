#ifndef MAINH
#define MAINH

#include <Arduino.h>
#include <Wifi.h>
#include <WiFiClient.h>
#include <MIDI.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include "settings.h"
#include "types.h"
#include "parser1.h"
#include "parser2.h"
#include "midiInterface.h"
#include "strMidiUtil.h"
#include "interface.h"
#include "admin.h"
#include "defaults.h"
#include "song.h"
#include "songUtil.h"

// for ESP8266 Use ESP8266WiFi.h instead of WiFi.

void mqttReconnect();
void wifiConnect();
void setup();
void loop();

extern bool playSongFlag;
extern bool parserV2;
extern uint8_t currentChanal;
extern uint32_t activeNotes[129];
extern uint32_t bpm;
extern uint32_t vierBeatZeit;
extern uint32_t timeout;
extern uint16_t zuletztGenannteNote;
extern notenBufferEintrag notenBuffer[NOTEN_BUFFER_LAENGE];
extern lied presetLieder[MENGE_PRESET_LIEDER];
extern instrument instrumente[MENGE_PRESET_INSTRUMENTE];
extern String song;
extern midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> MIDI;
extern WiFiClient wiFiClient;
extern PubSubClient psClient;

#endif
