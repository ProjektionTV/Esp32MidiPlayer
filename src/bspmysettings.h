#ifndef MY_SETTINGS_H_
#define MY_SETTINGS_H_

// uncomment the "#define" lines to set the value

//#define OTA_HOST "playmidi"
//#define OTA_PORT 0 // if 0 ignored

//#define MQTT_IRC_TX "irc/tx"
//#define MQTT_PLAYMIDI "playmidi"
//#define MQTT_CLIENT_ID "playmidi"
//#define MQTT_MUSIC_ON_TOPIC "stream/music"
//#define MQTT_MUSIC_ON_MESSAGE "on"
//#define MQTT_MUSIC_OFF_TOPIC MQTT_MUSIC_ON_TOPIC // if not defined: same as MQTT_MUSIC_ON_TOPIC
//#define MQTT_MUSIC_OFF_MESSAGE "off"
//#define MQTT_KILLMIDI "killmidi"

//#define INDEV 0
//#define DEBUG 0
//#define RUN_TEST 0 // if 1 test will be runn
//#define TEST_COUNT 5 // max count of test runs, canncled if all tests passed
//#define TEST_ANSI_COLOR 0 // if 1 test out will have ansi colors

//#define MAX_PLAYREQUESTS 0 // if 0 the que is unlimeted
//#define DEFAULT_MIDI_CHANNEL 1
//#define DEFAULT_BPM 240
//#define MAX_BUFFERS 0 // if 0 there is no limit of buffers
//#define MAX_TRACKS 0 // if 0 there is no limit to the track count
//#define MAX_TOKENS_PER_TICK 0 // if 0 there is no limit to the tokens parsed in a tick
//#define MIDI_STACK_SIZE 3

//#define MIDI_GM_BANK_MSB 0 // general midi bank
//#define MIDI_GM_BANK_LSB 0

//#define USE_PROJEKTION_FX 0 // if 1 projektionfx e1.31 output is enabled
//#define PROJEKTION_FX_LEDS 60 // num of leds projektion fx should output

#endif /* MY_SETTINGS_H_ */
