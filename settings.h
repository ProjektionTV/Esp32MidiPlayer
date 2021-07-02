#ifndef MAINSETTINGS
#define MAINSETTINGS

#define USE_ESP8266 0 // 0:Esp 32, 1:ESP 8266
#define SONG_NOT_EXISTS "H' E'"

#define IP_SUBNET 178
#define IP_BROKER 7
#define IP_CLIENT 203

#define OTA_HOST "playmidi"
#define OTA_PORT 0 // if 0 ignored

#define MQTT_IRC_TX "irc/tx"
#define MQTT_PLAYMIDI "playmidi"
#define MQTT_CLIENT_ID "playmidi"
#define MQTT_MUSIC_ON_TOPIC "stream/music"
#define MQTT_MUSIC_ON_MASSAGE "on"
#define MQTT_MUSIC_OFF_TOPIC MQTT_MUSIC_ON_TOPIC
#define MQTT_MUSIC_OFF_MASSAGE "off"

#define INDEV 0
#define DEBUG 0

#define MAX_PLAYREQUESTS 2
#define PING_TIMEOUT 120 * 1000
#define DEFALT_MIDI_CHANAL 1
#define DEFALT_BPM 240
#define ALLOW_MULTI_CHANAL_MIDI 1
#define ENABLE_PARSER_1_1 1
#define ALLOW_PARSER_2 1
#define NOTEN_BUFFER_LAENGE 8
#define MENGE_PRESET_LIEDER 16
#define MENGE_PRESET_INSTRUMENTE 8

#endif
