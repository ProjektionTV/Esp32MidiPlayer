#include "main.h"
#include "../../myauth.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

bool playSongFlag = false;
bool parserV2 = false;
uint8_t currentChanal = DEFALT_MIDI_CHANAL;
uint32_t activeNotes[129];
uint32_t bpm = DEFALT_BPM;
uint32_t vierBeatZeit = 1000;
uint32_t timeout = 0;
uint16_t zuletztGenannteNote = 2000;
notenBufferEintrag notenBuffer[NOTEN_BUFFER_LAENGE];
String presetLieder[MENGE_PRESET_LIEDER];
instrument instrumente[MENGE_PRESET_INSTRUMENTE];
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
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.begin();

  MIDI.begin(4);

  MIDI.sendProgramChange(0,currentChanal);
    
  psClient.setServer(mqttBroker, 1883);
  psClient.setBufferSize(4096);
  psClient.setCallback(mqttCallback);

  fuellePresetLieder();
  fuellePresetInstrumente();

  setMusicStatus(true);

  delay(500);

}

void playSong(String input, uint32_t timeOutSeconds){

    if(input.startsWith("~")){
      input.remove(0,1);
      if(isNumber(input.charAt(0))){
        uint32_t num = readNumber(input);
        if(num >= 0 && num < MENGE_PRESET_LIEDER)
          input = presetLieder[num];
      }
    }

    Serial.println("PLAY ERKANNT");

    currentChanal = DEFALT_MIDI_CHANAL;

    while (input.startsWith(" "))
      input.remove(0,1);

    if(input.startsWith("-")) {
#if ALLOW_PARSER_2
      parserV2 = true;
#else
      parserV2 = false;
#endif
      input.remove(0,1);
    } else {
      parserV2 = false;
    }

    while (input.startsWith(" "))
      input.remove(0,1);

    if(input.startsWith("bpm")){
      input.remove(0,3);
      if(isNumber(input.charAt(0))){
        bpm = readNumber(input);
      }else{
        bpm = DEFALT_BPM;
      }
    }else{
      bpm = DEFALT_BPM;
    }

    while (input.startsWith(" "))
      input.remove(0,1);
    
    readInstrument(input);

    vierBeatZeit = (int) ((float) (240000) / ((float) bpm));

    while (input.startsWith(" "))
      input.remove(0,1);
    delay(250);

    char * pch;

    pch = strtok((char*)input.c_str(), " ");

    timeout = millis() + timeOutSeconds * 1000;

    setMusicStatus(false);

    while ((pch != NULL) && (millis() < timeout)) {
      parserT(pch);
      pch = strtok (NULL, " ");
    }
    parser2allOFF();

    for(uint8_t i = 0; i < 17; i++){
       MIDI.sendProgramChange(0,i);
       MIDI.sendControlChange(7, 127, i);
       MIDI.sendControlChange(0, 0, i);
       MIDI.sendControlChange(32, 0, i);
       MIDI.sendControlChange(72, 63, i);
       MIDI.sendControlChange(73, 63, i);
    }
    setMusicStatus(true);
}

void loop()
{

  if(!WiFi.isConnected())
    esp_restart();

  if (!psClient.connected()) {
    mqttReconnect();
  }
  psClient.loop();
  ArduinoOTA.handle();

  if(playSongFlag)
  {
    playSongFlag = false;
    playSong(song, 600);
  }
}

void parserT(String buffer){
  if(buffer.charAt(0) == 'm' || buffer.charAt(0) == 'M'){
    buffer.remove(0,1);
#if ALLOW_PARSER_2
    parserV2 = !parserV2;
#endif
  }
  if(parserV2){
    parser2(buffer);
  }else{
#if ENABLE_PARSER_1_1
    parser1_1(buffer);
    parser2allOFF();
#else
    parser(buffer);
#endif
  }
}

void readInstrument(String &s){
    if (isNumber(s.charAt(0))){
      uint32_t midiInstrument = readNumber(s);
      if(midiInstrument < 128)
        MIDI.sendProgramChange(midiInstrument, currentChanal);
    }

    for(uint8_t i = 0; i < MENGE_PRESET_INSTRUMENTE; i++){
      if(s.startsWith(instrumente[i].name)){
        s.remove(0, instrumente[i].name.length());
        MIDI.sendProgramChange(instrumente[i].instrument,currentChanal);
        MIDI.sendControlChange(0, instrumente[i].bank_MSB, currentChanal);  //MSB
        MIDI.sendControlChange(32, instrumente[i].bank_LSB, currentChanal); //LSB
      }
    }
}
