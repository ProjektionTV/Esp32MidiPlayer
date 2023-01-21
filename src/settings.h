#ifndef MAINSETTINGS
#define MAINSETTINGS

#include "mysettings.h"

#ifndef SONG_NOT_EXISTS
#define SONG_NOT_EXISTS "H' E'"
#endif

#ifndef OTA_HOST
#define OTA_HOST "playmidi"
#endif
#ifndef OTA_PORT
#define OTA_PORT 0 // if 0 ignored
#endif

#ifndef MQTT_IRC_TX
#define MQTT_IRC_TX "irc/tx"
#endif
#ifndef MQTT_PLAYMIDI
#define MQTT_PLAYMIDI "playmidi"
#endif
#ifndef MQTT_CLIENT_ID
#define MQTT_CLIENT_ID "playmidi"
#endif
#ifndef MQTT_MUSIC_ON_TOPIC
#define MQTT_MUSIC_ON_TOPIC "stream/music"
#endif
#ifndef MQTT_MUSIC_ON_MESSAGE
#define MQTT_MUSIC_ON_MESSAGE "on"
#endif
#ifndef MQTT_MUSIC_OFF_TOPIC
#define MQTT_MUSIC_OFF_TOPIC MQTT_MUSIC_ON_TOPIC
#endif
#ifndef MQTT_MUSIC_OFF_MESSAGE
#define MQTT_MUSIC_OFF_MESSAGE "off"
#endif
#ifndef MQTT_KILLMIDI
#define MQTT_KILLMIDI "killmidi"
#endif

#ifndef LOOP_PULL_MS
#define LOOP_PULL_MS 2500
#endif

#ifndef INDEV
#define INDEV 0
#endif
#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef MAX_PLAYREQUESTS
#define MAX_PLAYREQUESTS 2
#endif
#ifndef DEFAULT_MIDI_CHANNEL
#define DEFAULT_MIDI_CHANNEL 1
#endif
#ifndef DEFAULT_BPM
#define DEFAULT_BPM 240
#endif
#ifndef ALLOW_MULTI_CHANNEL_MIDI
#define ALLOW_MULTI_CHANNEL_MIDI 1
#endif
#ifndef ENABLE_PARSER_1_1
#define ENABLE_PARSER_1_1 1
#endif
#ifndef ALLOW_PARSER_2
#define ALLOW_PARSER_2 1
#endif
#ifndef NOTES_BUFFER_LENGTH
#define NOTES_BUFFER_LENGTH 8
#endif
#ifndef AMOUNT_PRESET_SONGS
#define AMOUNT_PRESET_SONGS 16
#endif
#ifndef AMOUNT_PRESET_INSTRUMENTS
#define AMOUNT_PRESET_INSTRUMENTS 8
#endif

#endif
