#include "settings.h"
#if RUN_TEST

#define PROJEKTION_MIDI_TEST_ANSI TEST_ANSI_COLOR

#include "arduinoTestOutAdapter.hpp"
#include "core/test/tests.hpp"

int trys = 0;
bool ok = false;

adruinoTestOut *testOut;
silentAdruinoTestOut *silentTestOut;

void setup() {
  Serial.begin(115200);
  testOut = new adruinoTestOut();
  silentTestOut = new silentAdruinoTestOut();
  ok = !test(testOut);
  trys++;
  delay(10000);
}

void loop() {
  if(!ok && trys < TEST_COUNT) {
    Serial.println("--------------------------------------------------------------------------------");
    ok = !test(silentTestOut);
    trys++;
  } else if(!ok && trys == TEST_COUNT) { Serial.println("Canceling Tests!"); trys++; }
  delay(10000);
}

#else /* RUN_TEST */
#include "main.h"
#include "../../myauth.h"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <MIDI.h>
#include <WiFiClient.h>

#include "arduinoMidiAdapter.hpp"
#include "core/projektionMidi.hpp"
#include "defaults.h"
#include "interface.h"
#include "wifictrl.h"

#if USE_PROJEKTION_FX
#include "projektionFXIntigration.hpp"
#endif

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

arduinoMidiAdapter<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>> *midiAdapter;
projektionMidi::projektionMidi *projektionMidiPlayer;

#if useMqttBroakerIP
IPAddress mqttBrokerI(mqttBrokerIp0, mqttBrokerIp1, mqttBrokerIp2, mqttBrokerIp3);
#else
#define mqttBrokerI mqttBroker
#endif

WiFiClient wiFiClient;

PubSubClient psClient(wiFiClient);


void mqttReconnect() {
  // Loop until we're reconnected
  while (!psClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
#if noMqttUser
    if (psClient.connect(MQTT_CLIENT_ID))
#else
    if (psClient.connect(MQTT_CLIENT_ID, mqttUser, mqttPassword))
#endif
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //psClient.publish("wled/861a06/api","%PL=10");
      // ... and resubscribe
      psClient.subscribe(MQTT_PLAYMIDI, 1);
      psClient.subscribe(MQTT_KILLMIDI, 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(psClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  //Set up serial output with standard MIDI baud rate

  Serial.begin(115200);
  Serial.println();

  wifictrl.setupWifiPortal("MidiPlayer");

  ArduinoOTA.setHostname(OTA_HOST);
  if (OTA_PORT != 0)
    ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.begin();

  projektionMidiPlayer = new projektionMidi::projektionMidi();
  projektionMidiPlayer->setMusicStatusReciever(setMusicStatus);
  projektionMidiPlayer->setErrorReciever([](const char *msg) {
    size_t len = strlen(msg);
    const char *prefix = "(MIDI:ERROR) ";
    size_t prefixLen = strlen(prefix);
    char *out = (char *) malloc(len + prefixLen + 1);
    memcpy(out, prefix, prefixLen);
    memcpy(out + prefixLen, msg, len + 1);
    sendIrcMessage(out);
    Serial.printf("MIDI:ERR: %s\n", out);
    free(out);
  });
  projektionMidiPlayer->getSettings()->defaultBpm = DEFAULT_BPM;
  projektionMidiPlayer->getSettings()->maxQueueSize = MAX_PLAYREQUESTS;
  projektionMidiPlayer->getSettings()->maxBuffers = MAX_BUFFERS;
  projektionMidiPlayer->getSettings()->maxTracks = MAX_TRACKS;
  projektionMidiPlayer->getSettings()->maxTokensToParse = MAX_TOKENS_PER_TICK;
  projektionMidiPlayer->getSettings()->defaultMidiChannel = DEFAULT_MIDI_CHANNEL;
  projektionMidiPlayer->getSettings()->midiStackSize = MIDI_STACK_SIZE;
  projektionMidiPlayer->getSettings()->midi_gm_bank_msb = MIDI_GM_BANK_MSB;
  projektionMidiPlayer->getSettings()->midi_gm_bank_lsb = MIDI_GM_BANK_LSB;

  MIDI.begin(4);
  midiAdapter = new arduinoMidiAdapter(&MIDI, 16);
  midiAdapter->apply(projektionMidiPlayer, 1);

  psClient.setServer(mqttBrokerI, 1883);
  psClient.setBufferSize(9216);
  psClient.setCallback(mqttCallback);
  psClient.setKeepAlive(120);
  psClient.setSocketTimeout(120);  

  fillPresetSongs(projektionMidiPlayer);
  fillPresetInstruments(projektionMidiPlayer);

#if USE_PROJEKTION_FX
  projektionMidiPlayer->setFXHandler(&projektionFxHandler);
#endif

  setMusicStatus(true);

  delay(500);
}

void loop()
{
  wifictrl.check();

  if (!psClient.connected()) {
    mqttReconnect();
  }
  psClient.loop();
  ArduinoOTA.handle();

  projektionMidiPlayer->tick(micros64());
}
#endif /* RUN_TEST */
