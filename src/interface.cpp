#include "settings.h"
#include "interface.h"
#include "main.h"

#include <ArduinoJson.h>

void sendIrcMessage(String &s) {
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
  if(strTopic.equals(MQTT_PLAYMIDI)) {
    payload[length] = '\0';
    Serial.println("play midi vom mqtt erkannt");
    // JSON MIDI
    DynamicJsonDocument data(768);
    auto jsonErr = deserializeJson(data, payload);
    if(jsonErr != DeserializationError::Ok) {
      // backwards compatibility
      projektionMidiPlayer->enqueue((char*)payload, 16);
      return;
    }
    String midi = data["midi"];
    uint16_t length = data.containsKey("laenge") ? data["laenge"] : data["length"];

    bool allowBuffer = data.containsKey("aktiviereBuffer") ? data["aktiviereBuffer"] : data["allowBuffer"];
    if(allowBuffer) {
      String user = data.containsKey("nutzer") ? data["nutzer"] : data["user"];
      uint16_t maxBufferSize = data.containsKey("maximaleBufferGroesse") ? data["maximaleBufferGroesse"] : data["maxBufferSize"];

      uint32_t currBufferLength = 0;
      uint8_t out = projektionMidiPlayer->bufferOperation(midi.c_str(), user.c_str(), length, maxBufferSize, currBufferLength);
      if(out) {
        String text = "(MIDI) @" + user;
        bool sendText = false;

        // delete
        switch((out >> 0) & 3) {
        case 1:
          sendText = true;
          text += " Dein Puffer wurde erfolgreich gelöscht!";
          break;
        case 2:
          sendText = true;
          text += " Du hast keinen Puffer!";
          break;
        default:
          break;
        }
        // create
        switch((out >> 2) & 3) {
        case 1:
          sendText = true;
          text += " Puffer wurde erfolgreich erschaffen!";
          break;
        case 2:
          sendText = true;
          text += " Du hast bereits einen Puffer.";
          break;
        case 3:
          sendText = true;
          text += " Es konnte kein Puffer erschaffen werden.";
          break;
        default:
          break;
        }
        // modified
        if((out >> 4) & 1) {
          sendText = true;
          text += " Daten wurden zu deinem Puffer hinzugefügt (";
          text += currBufferLength;
          text += "/";
          text += maxBufferSize;
          text += ")";
        }
        // refund
        if((out >> 5) & 1) {
          if(data.containsKey("refundID") && data.containsKey("refundTopic")) {
            psClient.publish(data["refundTopic"], data["refundID"]);
          }
        }
        // faild enqueue
        if((out >> 6) & 1) {
          sendText = true;
          text += " Die Warteschlange ist voll!";
        }

        if(sendText) sendIrcMessage(text);
      }
    } else {
      if(!projektionMidiPlayer->enqueue(midi.c_str(), length)) {
        sendIrcMessage("(MIDI) Die Warteschlange ist voll!");
        if(data.containsKey("refundID") && data.containsKey("refundTopic")) {
          psClient.publish(data["refundTopic"], data["refundID"]);
        }
      }
    }
  } else if(strTopic.equals(MQTT_KILLMIDI)) {
    payload[length] = '\0';
    Serial.println("kill midi vom mqtt erkannt");
    projektionMidiPlayer->kill();
  }

}
