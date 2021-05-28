#include "interface.h"

void schreibeChatNachricht(String s) {
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

void setMusicStatus(bool newStatus){
  if(newStatus)
    psClient.publish(MQTT_MUSIC_ON_TOPIC, MQTT_MUSIC_ON_MASSAGE);
  else
    psClient.publish(MQTT_MUSIC_OFF_TOPIC, MQTT_MUSIC_OFF_MASSAGE);
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
    bool adminModus = data["adminModus"];
    if(adminModus){
      String midi = data["midi"];
      String nutzer = data["nutzer"];
      parseAdminCommand(midi, nutzer);
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
