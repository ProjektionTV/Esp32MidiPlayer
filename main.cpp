#include "../../myauth.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <MIDI.h>
#include <PubSubClient.h>

// for ESP8266 Use ESP8266WiFi.h instead of WiFi.h

IPAddress mqttBroker(192, 168, 178, 7);
IPAddress staticIP(192, 168, 178, 203);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 178, 1);

#define PING_TIMEOUT 120 * 1000
#define DEFALT_MIDI_CHANAL 1
#define DEFALT_BPM 240
#define MIDI_INSTRUMENT_piano 0
#define MIDI_INSTRUMENT_vibes 11
#define MIDI_INSTRUMENT_organ 19
#define MIDI_INSTRUMENT_guitar 30
#define MIDI_INSTRUMENT_brass 62
#define ALLOW_MULTI_CHANAL_MIDI 1
#define ENABLE_PARSER_1_1 1
#define ALLOW_PARSER_2 1

bool playSongFlag = false;
bool parserV2 = false;
uint8_t currentChanal = DEFALT_MIDI_CHANAL;
uint32_t songTimeoutSeconds = 16;  // Song time out in seconds (Maximum song length)
uint32_t activeNotes[129];
uint32_t bpm = DEFALT_BPM;
uint32_t vierBeatZeit = 1000;
String song;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void playMIDINote(byte channel, byte note, byte velocity);
void parser(String buffer);
void playNote(uint16_t note, uint8_t length);
void playSong(String input);
void mqttCallback(char* topic, byte* payload, unsigned int length);
void mqttReconnect();
void parser2(String buffer);
void parser1_1(String buffer);
void parser2note(uint16_t note);
void parser2allOFF();
bool isNumber(char c);
void playSong(String input, uint32_t timeOutSeconds);
uint8_t readOktaveOffset(String &s);
uint32_t readNumber(String &s);
void readHalbton(String &s, bool &halbtonC, bool &halbtonB);
uint16_t getNoteID(char note, bool &allowHabtonC, bool &allowHabtonB, bool &noteDown);
uint16_t convertNote(uint16_t noteId, uint8_t oktavenOffset, bool habtonC, bool habtonB, bool allowHabtonC, bool allowHabtonB, bool noteDown);


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



void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String strTopic = topic;
  if(strTopic.equals("playmidi"))
  {
    payload[length] = '\0';
    Serial.println("play midi vom mqtt erkannt");
    playSong((char*)payload);
  }

}


void mqttReconnect() {
  // Loop until we're reconnected
  while (!psClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (psClient.connect("playmidi")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //psClient.publish("wled/861a06/api","%PL=10");
      // ... and resubscribe
      psClient.subscribe("playmidi");
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


  MIDI.begin(4);

  MIDI.sendProgramChange(0,currentChanal);
    
  psClient.setServer(mqttBroker, 1883);
  psClient.setBufferSize(4096);
  psClient.setCallback(mqttCallback);

  delay(500);

}

void playSong(String input)
{
  playSong(input, songTimeoutSeconds);
}

void playSong(String input, uint32_t timeOutSeconds){

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
    
    if (isNumber(input.charAt(0))){
      uint32_t midiInstrument = readNumber(input);
      if(midiInstrument < 128)
        MIDI.sendProgramChange(midiInstrument, currentChanal);
    }

    if(input.startsWith("piano"))
    {
      input.remove(0,6);
      MIDI.sendProgramChange(MIDI_INSTRUMENT_piano,currentChanal);
    }

    if(input.startsWith("vibes"))
    {
      input.remove(0,6);
      MIDI.sendProgramChange(MIDI_INSTRUMENT_vibes,currentChanal);
    }

    if(input.startsWith("organ"))
    {
      input.remove(0,6);
      MIDI.sendProgramChange(MIDI_INSTRUMENT_organ,currentChanal);
    }

    if(input.startsWith("guitar"))
    {
      input.remove(0,7);
      MIDI.sendProgramChange(MIDI_INSTRUMENT_guitar,currentChanal);
    }

    if(input.startsWith("brass"))
    {
      input.remove(0,6);
      MIDI.sendProgramChange(MIDI_INSTRUMENT_brass,currentChanal);
    }

    vierBeatZeit = (int) ((float) (240000) / ((float) bpm));

    while (input.startsWith(" "))
      input.remove(0,1);
    delay(250);

    char * pch;

    pch = strtok((char*)input.c_str(), " ");

    uint32_t timeout = millis() + timeOutSeconds * 1000;

    if(parserV2){
      while ((pch != NULL) && (millis() < timeout)) {
        parser2(pch);
        pch = strtok (NULL, " ");
      }
      parser2allOFF();
    }

    while ((pch != NULL) && (millis() < timeout))
    {
#if ENABLE_PARSER_1_1
      parser1_1(pch);
      parser2allOFF();
#else
      parser(pch);
#endif
      pch = strtok (NULL, " ");
    }

    for(uint8_t i = 0; i < 17; i++)
       MIDI.sendProgramChange(MIDI_INSTRUMENT_piano,i);
}

void loop()
{

  if(!WiFi.isConnected())
    esp_restart();

  if (!psClient.connected()) {
    mqttReconnect();
  }
  psClient.loop();  

  if(playSongFlag)
  {
    playSongFlag = false;
    playSong(song);
  }
}

void playMIDINote(byte channel, byte note, byte velocity)
{
    //MIDI channels 1-16 are really 0-15
    byte noteOnStatus=0x90 + (channel-1);  
    
    //Send notes to MIDI output:
    Serial1.write(noteOnStatus);
    Serial1.write(note);
    Serial1.write(velocity);
}


void playNote(uint16_t note, uint8_t length)
{
  MIDI.sendNoteOn(note, 127, currentChanal);
  delay(vierBeatZeit/length);
  MIDI.sendNoteOff(note, 0, currentChanal);
}


void parser(String buffer)
{
  // C', C, c, c'
  Serial.printf("Parser: %s\n", buffer.c_str());

  char note = buffer.charAt(0);
  buffer.remove(0,1);

  int8_t oktaveOffset = 0;

  if(buffer.charAt(0) == '\'')
  {
    oktaveOffset++;
    buffer.remove(0,1);
  }

  if(buffer.charAt(0) == '\'')
  {
    oktaveOffset++;
    buffer.remove(0,1);
  }

  if(buffer.charAt(0) == '\'')
  {
    oktaveOffset++;
    buffer.remove(0,1);
  }

  bool istHalbton = 0;

  if(buffer.charAt(0) == '#')
  {
    istHalbton = true;
    buffer.remove(0,1);
  }

  uint8_t length = atoi(buffer.c_str());

  if(length==0)
    length=4;
  
  switch(note)
  {
    case 'P':
    case 'p':
        delay(1000/length);      
    break;
    case 'C':
      if(!istHalbton)
        playNote(48-(oktaveOffset*12),length);
      else
        playNote(49-(oktaveOffset*12),length);      
    break;
    case 'D':
      if(!istHalbton)
        playNote(50-(oktaveOffset*12),length);
      else
        playNote(51-(oktaveOffset*12),length);      
    break;
    case 'E':
        playNote(52-(oktaveOffset*12),length);      
    break;
    case 'F':
      if(!istHalbton)
        playNote(53-(oktaveOffset*12),length);
      else
        playNote(54-(oktaveOffset*12),length);      
    break;
    case 'G':
      if(!istHalbton)
        playNote(55-(oktaveOffset*12),length);
      else
        playNote(56-(oktaveOffset*12),length);      
    break;
    case 'A':
      if(!istHalbton)
        playNote(57-(oktaveOffset*12),length);
      else
        playNote(58-(oktaveOffset*12),length);      
    break;
    case 'H':
        playNote(59-(oktaveOffset*12),length);      
    break;

    case 'c':
      if(!istHalbton)
        playNote(60+(oktaveOffset*12),length);
      else
        playNote(61+(oktaveOffset*12),length);      
    break;
    case 'd':
      if(!istHalbton)
        playNote(62+(oktaveOffset*12),length);
      else
        playNote(63+(oktaveOffset*12),length);      
    break;
    case 'e':
        playNote(64+(oktaveOffset*12),length);      
    break;
    case 'f':
      if(!istHalbton)
        playNote(65+(oktaveOffset*12),length);
      else
        playNote(66+(oktaveOffset*12),length);      
    break;
    case 'g':
      if(!istHalbton)
        playNote(67+(oktaveOffset*12),length);
      else
        playNote(68+(oktaveOffset*12),length);      
    break;
    case 'a':
      if(!istHalbton)
        playNote(69+(oktaveOffset*12),length);
      else
        playNote(70+(oktaveOffset*12),length);      
    break;
    case 'h':
        playNote(71+(oktaveOffset*12),length);      
    break;
  }
}

void parser1_1(String buffer){
  Serial.printf("Parser1.1: %s\n", buffer.c_str());

  if(isNumber(buffer.charAt(0))){
    uint32_t length = readNumber(buffer);
    if(length == 0)
      length = 4;
    delay(vierBeatZeit/length);
    if(buffer.charAt(0) == '.'){
      delay((vierBeatZeit/length) / 2);
      buffer.remove(0,1);
    }
    return;
  }

  char note = buffer.charAt(0);
  buffer.remove(0,1);
  
  int8_t oktaveOffset = readOktaveOffset(buffer);
  bool habtonC = false;
  bool habtonB = false;
  readHalbton(buffer, habtonC, habtonB);
  bool noteDown = true;
  bool allowHabtonC = true;
  bool allowHabtonB = true;
  uint16_t noteID = getNoteID(note, allowHabtonC, allowHabtonB, noteDown);
  bool play = true;
  if(note == 'p' || note == 'P')
    play = false;
  if(play)
    parser2note(convertNote(noteID, oktaveOffset, habtonC, habtonB, allowHabtonC, allowHabtonB, noteDown));
  if(buffer.length() != 0)
    parser1_1(buffer); 
  else{
    delay(vierBeatZeit/4);
  }
}

void parser2(String buffer){
  Serial.printf("Parser2: %s\n", buffer.c_str());

  if(isNumber(buffer.charAt(0))){
    uint32_t length = readNumber(buffer);
    if(length == 0)
      length = 4;
    delay(vierBeatZeit/length);
    if(buffer.charAt(0) == '.'){
      delay((vierBeatZeit/length) / 2);
      buffer.remove(0,1);
    }
    if(buffer.length() != 0)
      parser2(buffer);
  }else{
    char note = buffer.charAt(0);
    buffer.remove(0,1);
    if(note == 's' || note == 'S'){
      parser2allOFF();
      if(buffer.length() != 0)
        parser2(buffer);
    }else if(note == 'i' || note == 'I'){
      if(buffer.startsWith("piano")){
        MIDI.sendProgramChange(MIDI_INSTRUMENT_piano,currentChanal);
        buffer.remove(0,5);
      }else if(buffer.startsWith("vibes")){
        MIDI.sendProgramChange(MIDI_INSTRUMENT_vibes,currentChanal);
        buffer.remove(0,5);
      }else if(buffer.startsWith("organ")){
        MIDI.sendProgramChange(MIDI_INSTRUMENT_organ,currentChanal);
        buffer.remove(0,5);
      }else if(buffer.startsWith("guitar")){
        MIDI.sendProgramChange(MIDI_INSTRUMENT_guitar,currentChanal);
        buffer.remove(0,6);
      }else if(buffer.startsWith("brass")){
        MIDI.sendProgramChange(MIDI_INSTRUMENT_brass,currentChanal);
        buffer.remove(0,5);
      }else {
        if(isNumber(buffer.charAt(0))){
          uint32_t ni = readNumber(buffer);
          MIDI.sendProgramChange(ni, currentChanal);
        }
      }
      if(buffer.length() != 0)
        parser2(buffer);
    }
#if ALLOW_MULTI_CHANAL_MIDI
    else if(note == 'k' || note == 'K'){
      if(isNumber(buffer.charAt(0))){
        uint32_t nc = readNumber(buffer);
        currentChanal = nc;
        if(buffer.length() != 0)
          parser2(buffer);
      }else 
        if(buffer.length() != 0)
          parser2(buffer);
    }
#endif
    else{
      int8_t oktaveOffset = readOktaveOffset(buffer);
      bool habtonC = false;
      bool habtonB = false;
      readHalbton(buffer, habtonC, habtonB);
      bool noteDown = true;
      bool allowHabtonC = true;
      bool allowHabtonB = true;
      uint16_t noteID = getNoteID(note, allowHabtonC, allowHabtonB, noteDown);
      parser2note(convertNote(noteID, oktaveOffset, habtonC, habtonB, allowHabtonC, allowHabtonB, noteDown));
      if(buffer.length() != 0)
        parser2(buffer);
    }
  }
}

void parser2note(uint16_t note){
  if(((activeNotes[note] >> currentChanal) & 1) != 1){
    //start note
    activeNotes[note] |= (1 << currentChanal);
    MIDI.sendNoteOn(note, 127, currentChanal);
  }else{
    //stop note
    activeNotes[note] &= ~(1 << currentChanal);
    MIDI.sendNoteOff(note, 0, currentChanal);
  }
}

void parser2allOFF(){
  for(uint16_t i = 0; i < 129; i++){
    if(activeNotes[i] != 0){
      for(uint8_t j = 0; j < 32; j++){
        if(((activeNotes[i] >> j) & 1) == 1){
          MIDI.sendNoteOff(i, 0, currentChanal);
        }
      }
      activeNotes[i] = 0;
    }
  }
}

bool isNumber(char c){
  switch(c){
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    return true;
  }
  return false;
}

uint8_t readOktaveOffset(String &s){
  uint8_t offset = 0;
  for(uint8_t i = 0; i < 3; i++){
    if(s.charAt(0) == '\''){
      offset++;
      s.remove(0,1);
    }
  }
  return offset;
}

uint16_t getNoteID(char note, bool &allowHabtonC, bool &allowHabtonB, bool &noteDown){
  uint16_t noteID = 0;
  switch(note) {
    case 'C':
      noteDown = true;
      allowHabtonB = false;
      allowHabtonC = true;
      noteID = 48;
    break;
    case 'D':
      noteDown = true;
      allowHabtonB = true;
      allowHabtonC = true;
      noteID = 50;
    break;
    case 'E':
      noteDown = true;
      allowHabtonB = true;
      allowHabtonC = false;
      noteID = 52;
    break;
    case 'F':
      noteDown = true;
      allowHabtonB = false;
      allowHabtonC = true;
      noteID = 53;
    break;
    case 'G':
      noteDown = true;
      allowHabtonB = true;
      allowHabtonC = true;
      noteID = 55;
    break;
    case 'A':
      noteDown = true;
      allowHabtonB = true;
      allowHabtonC = true;
      noteID = 57;
    break;
    case 'H':
      noteDown = true;
      allowHabtonB = true;
      allowHabtonC = false;
      noteID = 59;
    break;

    case 'c':
      noteDown = false;
      allowHabtonB = false;
      allowHabtonC = true;
      noteID = 60;
    break;
    case 'd':
      noteDown = false;
      allowHabtonB = true;
      allowHabtonC = true;
      noteID = 62;
    break;
    case 'e':
      noteDown = false;
      allowHabtonB = true;
      allowHabtonC = false;
      noteID = 64;
    break;
    case 'f':
      noteDown = false;
      allowHabtonB = false;
      allowHabtonC = true;
      noteID = 65;
    break;
    case 'g':
      noteDown = false;
      allowHabtonB = true;
      allowHabtonC = true;
      noteID = 67;
    break;
    case 'a':
      noteDown = false;
      allowHabtonB = true;
      allowHabtonC = true;
      noteID = 69;
    break;
    case 'h':
      noteDown = false;
      allowHabtonB = true;
      allowHabtonC = false;
      noteID = 71;
    break;
  }
  return noteID;
}

uint16_t convertNote(uint16_t noteId, uint8_t oktavenOffset, bool habtonC, bool habtonB, bool allowHabtonC, bool allowHabtonB, bool noteDown){
  return noteId + (oktavenOffset * 12 * (noteDown ? -1 : 1)) + (allowHabtonC && habtonC ? 1 : 0) - (allowHabtonB && habtonB ? 1 : 0);
}

void readHalbton(String &s, bool &halbtonC, bool &halbtonB){
  halbtonC = (s.charAt(0) == '#');
  if(halbtonC)
    s.remove(0,1);
  halbtonB = (s.charAt(0) == 'b');
  if(halbtonB)
    s.remove(0,1);
}

uint32_t readNumber(String &s){
  uint32_t curr = 0;
  while(isNumber(s.charAt(0))){
    curr *= 10;
    curr += s.charAt(0) - '0'; // get numerical value of number
    s.remove(0,1);
  }
  return curr;
}
