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
#include "parser1.h"
#include "parser2.h"
#include "midiInterface.h"
#include "strMidiUtil.h"
#include "interface.h"
#include "defaults.h"
#include "song.h"
#include "songUtil.h"

#ifdef ESP8266
#define esp_restart() []{ESP.restart();}
#endif

void mqttReconnect();
void wifiConnect();
void setup();
void loop();

extern bool playSongFlag;
extern bool parserV2;
extern bool inUserRequest;
extern uint8_t amountPlayRequestLeft;
extern uint32_t lastMqttCheck;
extern uint8_t currentChannel;
extern uint32_t activeNotes[129];
extern uint32_t bpm;
extern uint32_t fourBeatTime;
extern uint32_t timeout;
extern uint16_t lastNamedNote;
extern notesBufferEntry notesBuffer[NOTES_BUFFER_LENGTH];
extern presetSong presetSongs[AMOUNT_PRESET_SONGS];
extern instrument instruments[AMOUNT_PRESET_INSTRUMENTS];
extern openPlayRequest playRequests[MAX_PLAYREQUESTS]; 
extern String song;
extern midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> MIDI;
extern WiFiClient wiFiClient;
extern PubSubClient psClient;

#endif
