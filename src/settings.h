#ifndef MAINSETTINGS
#define MAINSETTINGS

#define SONG_NOT_EXISTS "H' E'"

#define OTA_HOST "playmidi"
#define OTA_PORT 0 // if 0 ignored

#define MQTT_IRC_TX "irc/tx"
#define MQTT_PLAYMIDI "playmidi"
#define MQTT_CLIENT_ID "playmidi"
#define MQTT_MUSIC_ON_TOPIC "stream/music"
#define MQTT_MUSIC_ON_MESSAGE "on"
#define MQTT_MUSIC_OFF_TOPIC MQTT_MUSIC_ON_TOPIC
#define MQTT_MUSIC_OFF_MESSAGE "off"
#define MQTT_KILLMIDI "killmidi"

#define LOOP_PULL_MS 2500

#define INDEV 0
#define DEBUG 0

#define MAX_PLAYREQUESTS 2
#define PING_TIMEOUT 120 * 1000
#define DEFAULT_MIDI_CHANNEL 1
#define DEFAULT_BPM 240
#define ALLOW_MULTI_CHANNEL_MIDI 1
#define ENABLE_PARSER_1_1 1
#define ALLOW_PARSER_2 1
#define NOTES_BUFFER_LENGTH 8
#define AMOUNT_PRESET_SONGS 16
#define AMOUNT_PRESET_INSTRUMENTS 8

#endif
