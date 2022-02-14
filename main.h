#ifndef MAINH
#define MAINH

#include "settings.h"
#include <Arduino.h>
#if USE_ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiClient.h>
#include <MIDI.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
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

#if USE_ESP8266
#define esp_restart() []{ESP.restart();}
#endif

void mqttReconnect();
void wifiConnect();
void setup();
void loop();

extern bool playSongFlag;
extern bool parserV2;
extern bool inUserRequest;
extern bool enabledUserDev;
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

extern uint8_t current_inst_i[17];
extern uint8_t current_inst_msb[17];
extern uint8_t current_inst_lsb[17];
extern uint8_t eepromData[256];
extern uint8_t curr_vol[17];

#endif
