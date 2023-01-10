#include "main.h"
#include "../../myauth.h"
#include "wifictrl.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

bool playSongFlag = false;
bool parserV2 = false;
bool inUserRequest = false;
uint8_t amountPlayRequestLeft = 0;
openPlayRequest playRequests[MAX_PLAYREQUESTS];
uint32_t lastMqttCheck = 0;
uint8_t currentChannel = DEFAULT_MIDI_CHANNEL;
uint32_t activeNotes[129];
uint32_t bpm = DEFAULT_BPM;
uint32_t fourBeatTime = 1000;
uint32_t timeout = 0;
uint16_t lastNamedNote = 2000;
notesBufferEntry notesBuffer[NOTES_BUFFER_LENGTH];
presetSong presetSongs[AMOUNT_PRESET_SONGS];
instrument instruments[AMOUNT_PRESET_INSTRUMENTS];
String song;

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
    if (psClient.connect(MQTT_CLIENT_ID)) {
#else
    if (psClient.connect(MQTT_CLIENT_ID, mqttUser, mqttPassword)) {
#endif
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

  MIDI.begin(4);

  for (uint8_t i = 1; i < 17; i++) {
    MIDI.sendProgramChange(0, i);
    MIDI.sendPitchBend(0, i);
  }
    
  psClient.setServer(mqttBrokerI, 1883);
  psClient.setBufferSize(9216);
  psClient.setCallback(mqttCallback);
  psClient.setKeepAlive(120);
  psClient.setSocketTimeout(120);  

  fillPresetSongs();
  fillPresetInstruments();

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
  lastMqttCheck = millis();
  ArduinoOTA.handle();

  if(!inUserRequest)
    while(amountPlayRequestLeft){
      String notes = playRequests[amountPlayRequestLeft - 1].data;
      uint32_t time = playRequests[amountPlayRequestLeft - 1].timeleft;
      playRequests[amountPlayRequestLeft - 1].data = "";
      playRequests[amountPlayRequestLeft - 1].timeleft = 0;
      amountPlayRequestLeft--;
      playSong(notes, time);
    }

  if(playSongFlag)
  {
    playSongFlag = false;
    playSong(song, 600);
  }
}
