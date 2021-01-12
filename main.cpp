#include "../../myauth.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <MIDI.h>
#include <PubSubClient.h>

IPAddress mqttBroker(192, 168, 178, 7);

#define PING_TIMEOUT 120 * 1000

bool playSongFlag = false;
String song;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void playMIDINote(byte channel, byte note, byte velocity);
void parser(String buffer);
void playNote(uint16_t note, uint8_t length);
void playSong(String input);
void mqttCallback(char* topic, byte* payload, unsigned int length);
void mqttReconnect();


WiFiClient wiFiClient;

PubSubClient psClient(wiFiClient);

void wifiConnect()
{
  IPAddress staticIP(192, 168, 178, 203);
  IPAddress gateway(192, 168, 178, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(192, 168, 178, 1);

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
  for (int i=0;i<length;i++) {
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

  MIDI.sendProgramChange(0,1);
    
  psClient.setServer(mqttBroker, 1883);
  psClient.setBufferSize(4096);
  psClient.setCallback(mqttCallback);

  delay(500);

}



void playSong(String input)
{
    uint32_t timeout = millis() + 16 * 1000;

    Serial.println("PLAY ERKANNT");

    if(input.startsWith("piano"))
    {
      input.remove(0,6);
      MIDI.sendProgramChange(0,1);
    }

    if(input.startsWith("vibes"))
    {
      input.remove(0,6);
      MIDI.sendProgramChange(11,1);
    }

    if(input.startsWith("organ"))
    {
      input.remove(0,6);
      MIDI.sendProgramChange(19,1);
    }

    if(input.startsWith("guitar"))
    {
      input.remove(0,7);
      MIDI.sendProgramChange(30,1);
    }

    if(input.startsWith("brass"))
    {
      input.remove(0,6);
      MIDI.sendProgramChange(62,1);
    }

    delay(500);

    char * pch;

    pch = strtok((char*)input.c_str(), " ");

    while ((pch != NULL) && (millis() < timeout))
    {
      parser(pch);
      pch = strtok (NULL, " ");
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
  MIDI.sendNoteOn(note, 127, 1);
  delay(1000/length);
  MIDI.sendNoteOff(note, 0, 1);
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
