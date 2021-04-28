#include "../../myauth.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <MIDI.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// for ESP8266 Use ESP8266WiFi.h instead of WiFi.h

IPAddress mqttBroker(192, 168, 178, 7);
IPAddress staticIP(192, 168, 178, 203);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 178, 1);

#define PING_TIMEOUT 120 * 1000
#define DEFALT_MIDI_CHANAL 1
#define DEFALT_BPM 240
#define ALLOW_MULTI_CHANAL_MIDI 1
#define ENABLE_PARSER_1_1 1
#define ALLOW_PARSER_2 1
#define NOTEN_BUFFER_LAENGE 8
#define MENGE_PRESET_LIEDER 6
#define MENGE_PRESET_INSTRUMENTE 5

struct notenBufferEintrag{
  uint8_t priority;
  String besitzer;
  String daten;
  uint16_t maximaleLaenge;
};

struct instrument{
  String name;
  uint8_t instrument;
  uint8_t bank_MSB;
  uint8_t bank_LSB;
};

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

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void playMIDINote(byte channel, byte note, byte velocity);
void parser(String buffer);
void playNote(uint16_t note, uint8_t length);
void mqttCallback(char* topic, byte* payload, unsigned int length);
void mqttReconnect();
void parser2(String buffer);
void parser1_1(String buffer);
void parserT(String buffer);
void parser2note(uint16_t note);
void parser2allOFF();
bool isNumber(char c);
void playSong(String input, uint32_t timeOutSeconds);
uint8_t readOktaveOffset(String &s);
uint32_t readNumber(String &s);
void readHalbton(String &s, bool &halbtonC, bool &halbtonB);
uint16_t getNoteID(char note, bool &allowHabtonC, bool &allowHabtonB, bool &noteDown);
uint16_t convertNote(uint16_t noteId, uint8_t oktavenOffset, bool habtonC, bool habtonB, bool allowHabtonC, bool allowHabtonB, bool noteDown);
void schreibeChatNachricht(String s);
void readInstrument(String &s);
void fuellePresetLieder();
void fuellePresetInstrumente();


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
    //rueckwertz kompatiblitaet
    if(payload[0] != '{'){
      playSong((char*)payload,16);
      return;
    }
    //JSON MIDI
    DynamicJsonDocument data(768);
    deserializeJson(data, payload);
    bool erlaubeBuffer = data["aktiviereBuffer"];
    if(erlaubeBuffer){
      String midi = data["midi"];
      String nutzer = data["nutzer"];
      //buffer funktionen
      if(midi.startsWith(";")){
        midi.remove(0, 1);
        // loesche buffer aktion
        if(midi.startsWith("l")){
          midi.remove(0, 1);
          bool wurdeGeloescht = false;
          for(uint8_t i = 0; i < NOTEN_BUFFER_LAENGE; i++){
            if(nutzer.equalsIgnoreCase(notenBuffer[i].besitzer)){
              notenBuffer[i].besitzer = "";
              notenBuffer[i].daten = "";
              notenBuffer[i].maximaleLaenge = 0;
              notenBuffer[i].priority = 0;
              wurdeGeloescht = true;
            }
          }
          if(wurdeGeloescht){
            schreibeChatNachricht("(MIDI) @" + nutzer + " dein Puffer wurde erfolgreich gelöscht!");
          }else{
            schreibeChatNachricht("(MIDI) @" + nutzer + " du hast keinen Puffer!");
          }
        }
        bool benutzerBufferGefunden = false;
        uint8_t bufferID;
        for(uint8_t i = 0; i < NOTEN_BUFFER_LAENGE; i++){
          if(nutzer.equalsIgnoreCase(notenBuffer[i].besitzer)){
            benutzerBufferGefunden = true;
            bufferID = i;
          }
        }
        if(benutzerBufferGefunden){
          // daten zum buffer hinzu fügen
          if(notenBuffer[bufferID].daten.length() == notenBuffer[bufferID].maximaleLaenge){
            schreibeChatNachricht("(MIDI) @" + nutzer + " dein Puffer ist Voll!");
          }else if ((notenBuffer[bufferID].daten.length() + midi.length()) > notenBuffer[bufferID].maximaleLaenge){
            notenBuffer[bufferID].daten = notenBuffer[bufferID].daten + midi + " ";
            notenBuffer[bufferID].daten = notenBuffer[bufferID].daten.substring(0, notenBuffer[bufferID].maximaleLaenge);
            schreibeChatNachricht("(MIDI) @" + nutzer + " daten wurden zu deinem Puffer hinzugefügt. Achtung es wurden Daten entfernt da der puffer überfüllt wurde (" + notenBuffer[bufferID].daten.length() + "/" + notenBuffer[bufferID].maximaleLaenge + ").");
          }else{
            notenBuffer[bufferID].daten = notenBuffer[bufferID].daten + midi + " ";
            schreibeChatNachricht("(MIDI) @" + nutzer + " daten wurden zu deinem Puffer hinzugefügt (" + notenBuffer[bufferID].daten.length() + "/" + notenBuffer[bufferID].maximaleLaenge + ").");
          }
        }else{
          if(midi.startsWith("n")){
            midi.remove(0, 1);
            uint8_t prioritaet = data["prioritaet"];
            // erschaffe neuen buffer
            uint8_t bufferID = 0;
            bool erschaffeBuffer = false;
            bool ueberSchreibeBuffer = true;
            for(uint8_t i = 0; i < NOTEN_BUFFER_LAENGE; i++){
              if(notenBuffer[i].besitzer.equalsIgnoreCase("") && ueberSchreibeBuffer){
                //erschaffe neuen buffer
                ueberSchreibeBuffer = false;
                erschaffeBuffer = true;
                bufferID = i;
              }
            }
            if(ueberSchreibeBuffer){
              for(uint8_t i = 0; i < NOTEN_BUFFER_LAENGE; i++){
                if(notenBuffer[i].priority < prioritaet && !(ueberSchreibeBuffer)){
                  //erschaffe neuen buffer
                  ueberSchreibeBuffer = true;
                  erschaffeBuffer = true;
                  bufferID = i;
                }
              }
            }
            if(erschaffeBuffer){
              uint16_t maximaleBufferGroesse = data["maximaleBufferGroesse"];
              notenBuffer[bufferID].besitzer = nutzer;
              notenBuffer[bufferID].priority = prioritaet;
              notenBuffer[bufferID].maximaleLaenge = maximaleBufferGroesse;
              notenBuffer[bufferID].daten = midi + " ";
              if(notenBuffer[bufferID].daten.length() > notenBuffer[bufferID].maximaleLaenge){
                notenBuffer[bufferID].daten = notenBuffer[bufferID].daten.substring(0, notenBuffer[bufferID].maximaleLaenge);
              }
              //TODO: ueberscheube einen buffer
              schreibeChatNachricht("(MIDI) @" + nutzer + " puffer wurde erfolgreich erschaffen (" + notenBuffer[bufferID].daten.length() + "/" + notenBuffer[bufferID].maximaleLaenge + ").");
            }else{
              schreibeChatNachricht("(MIDI) @" + nutzer + " puffer konte nicht erschaffen werden.");
            }
          }else{
            // spiele daten
            playSong(midi, (uint32_t) data["laenge"]);
          }
        }
      }else{
        bool benutzerBufferGefunden = false;
        for(uint8_t i = 0; i < NOTEN_BUFFER_LAENGE; i++){
          if(nutzer.equalsIgnoreCase(notenBuffer[i].besitzer)){
            benutzerBufferGefunden = true;
            playSong(notenBuffer[i].daten + midi, (uint32_t) data["laenge"]);
            notenBuffer[i].besitzer = "";
            notenBuffer[i].daten = "";
            notenBuffer[i].maximaleLaenge = 0;
            notenBuffer[i].priority = 0;
          }
        }
        if(!benutzerBufferGefunden)
          playSong(midi, (uint32_t) data["laenge"]);
      }
    }else{
      playSong(data["midi"], (uint32_t) data["laenge"]);
    }
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

  fuellePresetLieder();
  fuellePresetInstrumente();

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
    playSong(song, 600);
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

void parser1_1(String buffer){
  Serial.printf("Parser1.1: %s\n", buffer.c_str());

  if(millis() > timeout)
    return;

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
  if(noteID == 2000)
    play = false;
  if(note == 'p' || note == 'P')
    play = false;
  if(play)
    parser2note(convertNote(noteID, oktaveOffset, habtonC, habtonB, allowHabtonC, allowHabtonB, noteDown));
  if(buffer.length() != 0)
    parser1_1(buffer); 
  else{
    if(play || note == 'p' || note == 'P')
      delay(vierBeatZeit/4);
  }
}

void parser2(String buffer){
  Serial.printf("Parser2: %s\n", buffer.c_str());

  if(millis() > timeout)
    return;

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
    }else if(note == 'l' || note == 'L'){
      if(zuletztGenannteNote != 2000){
        parser2note(zuletztGenannteNote);
      }
      if(buffer.length() != 0)
        parser2(buffer);
    }else if(note == 'v' || note == 'V'){
      if(isNumber(buffer.charAt(0))){
        uint32_t nv = readNumber(buffer);
        if(nv < 128 && nv >= 0)
          MIDI.sendControlChange(7, nv, currentChanal);
      }
      if(buffer.length() != 0)
        parser2(buffer);
    }else if(note == 'x' || note == 'X'){
      if(isNumber(buffer.charAt(0))){
        uint32_t nv = readNumber(buffer);
        if(nv < 128 && nv >= 0)
          MIDI.sendControlChange(0, nv, currentChanal);
      }
      if(buffer.length() != 0)
        parser2(buffer);
    }else if(note == 'y' || note == 'Y'){
      if(isNumber(buffer.charAt(0))){
        uint32_t nv = readNumber(buffer);
        if(nv < 128 && nv >= 0)
          MIDI.sendControlChange(32, nv, currentChanal);
      }
      if(buffer.length() != 0)
        parser2(buffer);
    }else if(note == 'j' || note == 'J'){
      if(isNumber(buffer.charAt(0))){
        uint32_t nv = readNumber(buffer);
        if(nv < 128 && nv >= 0)
          MIDI.sendControlChange(72, nv, currentChanal);
      }
      if(buffer.length() != 0)
        parser2(buffer);
    }else if(note == 'o' || note == 'O'){
      if(isNumber(buffer.charAt(0))){
        uint32_t nv = readNumber(buffer);
        if(nv < 128 && nv >= 0)
          MIDI.sendControlChange(73, nv, currentChanal);
      }
      if(buffer.length() != 0)
        parser2(buffer);
    }else if(note == 'i' || note == 'I'){
      readInstrument(buffer);
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
      if(noteID != 2000)
        parser2note(convertNote(noteID, oktaveOffset, habtonC, habtonB, allowHabtonC, allowHabtonB, noteDown));
      if(buffer.length() != 0)
        parser2(buffer);
    }
  }
}

void parser2note(uint16_t note){
  zuletztGenannteNote = note;
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
  zuletztGenannteNote = 2000;
  for(uint16_t i = 0; i < 129; i++){
    if(activeNotes[i] != 0){
      for(uint8_t j = 0; j < 32; j++){
        if(((activeNotes[i] >> j) & 1) == 1){
          MIDI.sendNoteOff(i, 0, j);
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
  uint16_t noteID = 2000;
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

void fuellePresetLieder(){
  presetLieder[0] = "118 c4 C'4 mi119 c'2";
  presetLieder[1] = "brass F2 G#2 F4 F8 A#4 F4 D# F2 c2 F4 F8 c# c4 G# F4 c4 f4 F4 D# D#8 C G# F1";
  presetLieder[2] = "-126 c1 1 1 1 1 1";
  presetLieder[3] = "-bpm160 piano DD'f'8f'd'8d'a8ad'8d'f'8f'd'8d'a8ad'8s FF'f'8f'c'8c'a8ac'8c'f'8f'c'8c'a8ac'8s AA'e'8e'c'#8c'#a8ac'#8c'#e'8e'c'#8c'#a8ac'#8s AA'e'8e'c'#8c'#a8ac'#8s AA'e'4e'a'4s dd'D'8D'A'8A'D8DA'8A'F8FD8see'A'8A'D8s " +
    "f'afD'8D'A'8A'F8FA'8A'F8FD8DA'8A'D8s a'c'aF'8F'C'8C'F8sg'gC'8C'A8AF8sa'aC'8C'F8s cc'F'8F'C'8C'F8FC'8C'A8AF8FC'8C'F8s dd'H'b8H'bF8FHb8HbFs d8sHb8s ee'F8FHb8s ff'H'b8H'bF8FHb8HbF8s e'd8dHb8HbF8FHb8Hbs g'c'gC8CG8Gc8cG8Gs aa'e8ec8cG8GC8s gg'C8CE8EA8AE8s " +
    "ff'c#8c#A8AE8EA'8s D'8sA'8s ff'D8DA'8s ff'D'8D'A'8s ff'D8DA'8s aa'D'8D'A'8saa'D8DA'8s gg'D'8D'A'8sff'E'8E'A'8s F'8sC8saa'F8FC8saa'F'8F'C8saa'F8FC8s gg'F'8F'C8saa'F8FC8s gg'F'8F'C8sff'F8FC8s H'b8sF8sff'Hb8HbF8sff'H'b8H'bF8sff'Hb8HbF8s " +
    "aa'H'b8H'bF8saa'Hb8HbF8sgg'H'b8H'bF8sff'Hb8HbF8s C8sG8saa'c8cG8saa'C8CG8saa'H'b8H'bG8s A'8sE8sc''#c'#A8AE8sc'#c''#A'8A'E8sc'#c''#A8AE8s D'8sA'8sff'D8DA'8sff'D'8D'A'8sff'D'8D'A'8s aa'F'8F'C8saa'F8FC8s gg'F'8F'C8sff'F8FA'8s " +
    "H'b8sF8shbh'bHb8HbF8shbh'bH'b8H'bF8shbh'bHb8HbF8s gg'C8CG8Gc8cG8sc'c''C8CG8Gc8cG8s aa'c'#A'8A'E8EA8AE8sc'c''#e'A'8A'E8EA4s DAdaf'8af'd'8d'f'8f'a'8a'e'8e'c'#8c'#a'8a'c''#8s d''d'''2D'";
  presetLieder[4] = "bpm120 41 gg'16 GG'16 gG16 GG'16 gG16 GG'16 gG16 GG'16 gG16 GG'16 gG16 GG'16 ff'16 GG'16 gG16 GG'16 gG16 GG'16 gG16 GG'16 gG16 GG'16 gG16 GG'16 ebe'b16 GG'16 gG16 GG'16 gG16 GG'16 gG16 GG'16 gG16 GG'16 gG16 GG'16 dd'16 GG'16 gG16 GG'16 " +
    "gG16 GG'16 gG16 GG'16 gG16 p8. p1 dd'16 DD'16 dD16 DD'16 dD16 DD'16 dD16 DD'16 dD16 DD'16 dD16 DD'16 cc'16 DD'16 dD16 DD'16 dD16 DD'16 dD16 DD'16 dD16 DD'16 dD16 DD'16 hbh'b16 DD'16 dD16 DD'16 dD16 DD'16 dD16 DD'16 dD16 DD'16 dD16 DD'16 " +
    "aA16 DD'16 dD16 DD'16 dD16 DD'16 dD16 DD'16 dD16 p8. p16 d16 c16 Hb16 A16 G16 F#16 E16 D16 C16 H'b16 A'16 G'G''16 d16 G'G''c16 Hb16 G'G''A16 G16 G'G''F#16 E16 G'G''D16 C16 G'G''H'b16 A16 G'G''16 g16 G'G''f16 eb16 dG'G''16 c16 HbG'G''16 A16 " +
    "GG'G''16 F16 EbG'G''16 D16 CC'16 g16 CC'f16 eb16 dCC'16 c16 HbCC'16 A16 GCC'16 F16 EbCC'16 D16 CC'16 hb16 CC'a16 g16 CC'f16 eb16 CC'd16 c16 CC'Hb16 A16 CC'G16 F16 EbE'b16 hb16 EbE'ba16 g16 EbE'bf16 eb16 EbE'bd16 c16 EbE'bHb16 A16 EbE'bG16 F16 " + 
    "EbE'b16 d'16 EbE'bc'16 hb16 EbE'ba16 g16 EbE'bf#16 e16 EbE'bd16 c16 EbE'bHb16 A16 GG'16 d'16 GG'c'16 hb16 GG'a16 g16 GG'f#16 e16 GG'd16 c16 GG'Hb16 A16 GG'16 d'16 GG'c'16 hb16 GG'a16 g16 GG'f#16 e16 GG'd16 c16 GG'Hb16 A16 GG'16 d'16 GG'c'16 hb16 GG'a16 g16 " +
    "GG'f#16 e16 GG'd16 c16 GG'Hb16 A16 dd'16 GG'16 GG'16 GG'16 dd'16 GG'16 GG'16 GG'16 dd'16 GG'16 GG'16 GG'16 D'16 DA16 D'Hb16 Dc16 D'd16 De16 D'f#16 Dg16 D'a16 D16 D'16 D16 D'16 CA16 D'Hb16 Cc16 D'd16 Ce16 D'f#16 Cg16 D'a16 C16 D'16 C16 ";
  presetLieder[5] = "piano H8 A8 H2 E2 p4 c8 H8 c4 H4 A2 p4 c8 H8 c2 E2 p4 A8 G8 A4 G4 F#4 A4 p4 H8 A8 H2 E2 F#8 G8 A2. G8 A8 H4 A4 G4 F#4 E2 c2 H2 H4 c2 H8 A8 H1 E4 E4 F#4 G4 F2 E2. p2 E4 E4 F#4 G4 E1 E4 E4 F#4 G4 F#4 E4 E2 p2 G4 F#2 E4 D4 D1 p2 c8 H8 c4 h4 a2" +
    "p4 c8 H8 c2 E2 p4 A8 G8 A4 G4 F#4 A4 G2. F#8 G8 A2. G8 A8 H4 A4 G4 F#4 E2 c2 H2. H8 c8 H8 A8 H1";
}

void fuellePresetInstrumente(){
  instrumente[0].name = "piano";
  instrumente[0].instrument = 0;
  instrumente[0].bank_MSB = 0;
  instrumente[0].bank_LSB = 0;

  instrumente[1].name = "vibes";
  instrumente[1].instrument = 11;
  instrumente[1].bank_MSB = 0;
  instrumente[1].bank_LSB = 0;

  instrumente[2].name = "organ";
  instrumente[2].instrument = 19;
  instrumente[2].bank_MSB = 0;
  instrumente[2].bank_LSB = 0;

  instrumente[3].name = "guitar";
  instrumente[3].instrument = 30;
  instrumente[3].bank_MSB = 0;
  instrumente[3].bank_LSB = 0;

  instrumente[4].name = "brass";
  instrumente[4].instrument = 62;
  instrumente[4].bank_MSB = 0;
  instrumente[4].bank_LSB = 0;
}

void schreibeChatNachricht(String s){
  psClient.publish("irc/tx", s.c_str());
}
