#include "interface.h"
#include "main.h"

void sendIrcMessage(String s) {
  if (s.length() > 500) {
    while(s.length() > 500){
      psClient.publish(MQTT_IRC_TX, s.substring(0, 500).c_str());
      s.remove(0, 500);
      if (s.length() > 0) {
        psClient.publish(MQTT_IRC_TX, s.c_str());
      }
    }
  } else
    psClient.publish(MQTT_IRC_TX, s.c_str());
}

void sendIrcMessage(const char *s) {
  size_t len = strlen(s);
  if (len > 500) {
    size_t i = 0;
    while((len - i) >= 500) {
      psClient.publish(MQTT_IRC_TX, s + i, 500);
      i += 500;
    }
    if(len - i) psClient.publish(MQTT_IRC_TX, s + i, len - i);
  } else
    psClient.publish(MQTT_IRC_TX, s);
}

void setMusicStatus(bool newStatus){
  if(newStatus)
    psClient.publish(MQTT_MUSIC_ON_TOPIC, MQTT_MUSIC_ON_MESSAGE);
  else
    psClient.publish(MQTT_MUSIC_OFF_TOPIC, MQTT_MUSIC_OFF_MESSAGE);
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
  if(strTopic.equals(MQTT_PLAYMIDI))
  {
    payload[length] = '\0';
    Serial.println("play midi vom mqtt erkannt");
    //JSON MIDI
    DynamicJsonDocument data(768);
    auto jsonErr = deserializeJson(data, payload);
    if(jsonErr != DeserializationError::Ok) {
      // backwards compatibility
      projektionMidiPlayer->enqueue((char*)payload, 16);
      return;
    }
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
          for(uint8_t i = 0; i < NOTES_BUFFER_LENGTH; i++){
            if(nutzer.equalsIgnoreCase(notesBuffer[i].owner)){
              notesBuffer[i].owner = "";
              notesBuffer[i].data = "";
              notesBuffer[i].maxLength = 0;
              notesBuffer[i].priority = 0;
              wurdeGeloescht = true;
            }
          }
          if(wurdeGeloescht){
            sendIrcMessage("(MIDI) @" + nutzer + " dein Puffer wurde erfolgreich gelöscht!");
          }else{
            sendIrcMessage("(MIDI) @" + nutzer + " du hast keinen Puffer!");
          }
        }
        bool benutzerBufferGefunden = false;
        uint8_t bufferID;
        for(uint8_t i = 0; i < NOTES_BUFFER_LENGTH; i++){
          if(nutzer.equalsIgnoreCase(notesBuffer[i].owner)){
            benutzerBufferGefunden = true;
            bufferID = i;
          }
        }
        if(benutzerBufferGefunden){
          // daten zum buffer hinzu fügen
          if(notesBuffer[bufferID].data.length() == notesBuffer[bufferID].maxLength){
            sendIrcMessage("(MIDI) @" + nutzer + " dein Puffer ist Voll!");
          }else if ((notesBuffer[bufferID].data.length() + midi.length()) > notesBuffer[bufferID].maxLength){
            notesBuffer[bufferID].data = notesBuffer[bufferID].data + midi + " ";
            notesBuffer[bufferID].data = notesBuffer[bufferID].data.substring(0, notesBuffer[bufferID].maxLength);
            sendIrcMessage("(MIDI) @" + nutzer + " daten wurden zu deinem Puffer hinzugefügt. Achtung es wurden Daten entfernt da der puffer überfüllt wurde (" + notesBuffer[bufferID].data.length() + "/" + notesBuffer[bufferID].maxLength + ").");
          }else{
            notesBuffer[bufferID].data = notesBuffer[bufferID].data + midi + " ";
            sendIrcMessage("(MIDI) @" + nutzer + " daten wurden zu deinem Puffer hinzugefügt (" + notesBuffer[bufferID].data.length() + "/" + notesBuffer[bufferID].maxLength + ").");
          }
        }else{
          if(midi.startsWith("n")){
            midi.remove(0, 1);
            uint8_t prioritaet = data["prioritaet"];
            // erschaffe neuen buffer
            uint8_t bufferID = 0;
            bool erschaffeBuffer = false;
            bool ueberSchreibeBuffer = true;
            for(uint8_t i = 0; i < NOTES_BUFFER_LENGTH; i++){
              if(notesBuffer[i].owner.equalsIgnoreCase("") && ueberSchreibeBuffer){
                //erschaffe neuen buffer
                ueberSchreibeBuffer = false;
                erschaffeBuffer = true;
                bufferID = i;
              }
            }
            if(ueberSchreibeBuffer){
              for(uint8_t i = 0; i < NOTES_BUFFER_LENGTH; i++){
                if(notesBuffer[i].priority < prioritaet && !(ueberSchreibeBuffer)){
                  //erschaffe neuen buffer
                  ueberSchreibeBuffer = true;
                  erschaffeBuffer = true;
                  bufferID = i;
                }
              }
            }
            if(erschaffeBuffer){
              uint16_t maximaleBufferGroesse = data["maximaleBufferGroesse"];
              notesBuffer[bufferID].owner = nutzer;
              notesBuffer[bufferID].priority = prioritaet;
              notesBuffer[bufferID].maxLength = maximaleBufferGroesse;
              notesBuffer[bufferID].data = midi + " ";
              if(notesBuffer[bufferID].data.length() > notesBuffer[bufferID].maxLength){
                notesBuffer[bufferID].data = notesBuffer[bufferID].data.substring(0, notesBuffer[bufferID].maxLength);
              }
              //TODO: ueberschreibe einen buffer
              sendIrcMessage("(MIDI) @" + nutzer + " puffer wurde erfolgreich erschaffen (" + notesBuffer[bufferID].data.length() + "/" + notesBuffer[bufferID].maxLength + ").");
            }else{
              sendIrcMessage("(MIDI) @" + nutzer + " puffer konte nicht erschaffen werden.");
            }
          }else{
            // spiele daten
            projektionMidiPlayer->enqueue(midi.c_str(), data["laenge"]);
          }
        }
      }else{
        bool benutzerBufferGefunden = false;
        for(uint8_t i = 0; i < NOTES_BUFFER_LENGTH; i++){
          if(nutzer.equalsIgnoreCase(notesBuffer[i].owner)){
            benutzerBufferGefunden = true;
            projektionMidiPlayer->enqueue((notesBuffer[i].data + midi).c_str(), data["laenge"]);
            notesBuffer[i].owner = "";
            notesBuffer[i].data = "";
            notesBuffer[i].maxLength = 0;
            notesBuffer[i].priority = 0;
          }
        }
        if(!benutzerBufferGefunden)
          projektionMidiPlayer->enqueue(midi.c_str(), data["laenge"]);
      }
    }else{
      projektionMidiPlayer->enqueue(data["midi"], data["laenge"]);
    }
  }else if(strTopic.equals(MQTT_KILLMIDI)){
    payload[length] = '\0';
    Serial.println("kill midi vom mqtt erkannt");
    projektionMidiPlayer->kill();
  }

}
