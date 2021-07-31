#include "main.h"
#include "../../myauth.h"

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

IPAddress mqttBroker(192, 168, IP_SUBNET, IP_BROKER);
IPAddress staticIP(192, 168, IP_SUBNET, IP_CLIENT);
IPAddress gateway(192, 168, IP_SUBNET, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, IP_SUBNET, 1);

WiFiClient wiFiClient;

PubSubClient psClient(wiFiClient);

void wifiConnect()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed.");
  }

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    if(i++>30)
      esp_restart();
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS 1: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS 2: ");
  Serial.println(WiFi.dnsIP(1));

}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!psClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (psClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //psClient.publish("wled/861a06/api","%PL=10");
      // ... and resubscribe
      psClient.subscribe(MQTT_PLAYMIDI);
      psClient.subscribe(MQTT_KILLMIDI);
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

  wifiConnect();

  ArduinoOTA.setHostname(OTA_HOST);
  if (OTA_PORT != 0)
    ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.begin();

  MIDI.begin(4);

  for (uint8_t i = 1; i < 17; i++) {
    MIDI.sendProgramChange(0, i);
    MIDI.sendPitchBend(0, i);
  }
    
  psClient.setServer(mqttBroker, 1883);
  psClient.setBufferSize(4096);
  psClient.setCallback(mqttCallback);

  fillPresetSongs();
  fillPresetInstruments();

  setMusicStatus(true);

  delay(500);

}

void loop()
{

  if(!WiFi.isConnected())
    esp_restart();

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
