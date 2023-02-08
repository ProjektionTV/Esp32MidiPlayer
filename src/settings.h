#ifndef MAINSETTINGS
#define MAINSETTINGS

#include "mysettings.h"

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

#ifndef INDEV
#define INDEV 0
#endif
#ifndef DEBUG
#define DEBUG 0
#endif
#ifndef RUN_TEST
#define RUN_TEST 0
#endif
#ifndef TEST_COUNT
#define TEST_COUNT 5
#endif
#ifndef TEST_ANSI_COLOR
#define TEST_ANSI_COLOR 0
#endif

#ifndef MAX_PLAYREQUESTS
#define MAX_PLAYREQUESTS 0
#endif
#ifndef DEFAULT_MIDI_CHANNEL
#define DEFAULT_MIDI_CHANNEL 1
#endif
#ifndef DEFAULT_BPM
#define DEFAULT_BPM 240
#endif
#ifndef MAX_BUFFERS
#define MAX_BUFFERS 0
#endif
#ifndef MAX_TRACKS
#define MAX_TRACKS 0
#endif

#ifndef USE_PROJEKTION_FX
#define USE_PROJEKTION_FX 0
#endif
#ifndef PROJEKTION_FX_LEDS
#define PROJEKTION_FX_LEDS 60
#endif

#endif
