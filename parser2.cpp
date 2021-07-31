#include "parser2.h"

void parser2(String buffer){
  Serial.printf("Parser2: %s\n", buffer.c_str());

  while (buffer.length() > 0){
    if(millis() > timeout)
      return;

    if((lastMqttCheck + LOOP_PULL_MS) < millis())
      loop();
    if (!psClient.connected())
      mqttReconnect();
    psClient.loop();

    if(isNumber(buffer.charAt(0))){
      uint32_t length = readNumber(buffer);
      if(length == 0)
        length = 4;
      midiDelay(fourBeatTime/length);
      if(buffer.charAt(0) == '.'){
        midiDelay((fourBeatTime/length) / 2);
        buffer.remove(0,1);
      }
    }else{
      char note = buffer.charAt(0);
      buffer.remove(0,1);
      if(note == 's' || note == 'S'){
        parser2allOFF();
      }else if(note == 'l' || note == 'L'){
        if(lastNamedNote != 2000){
          parser2note(lastNamedNote);
        }
      }else if(note == 'q' || note == 'Q'){
        if(isNumber(buffer.charAt(0))){
          uint32_t nv = readNumber(buffer);
          fourBeatTime = (int) ((float) (240000) / ((float) nv));
        }
      }else if(note == 'v' || note == 'V'){
        if(isNumber(buffer.charAt(0))){
          uint32_t nv = readNumber(buffer);
          if(nv < 128 && nv >= 0)
            MIDI.sendControlChange(7, nv, currentChannel);
        }
      }else if(note == 'x' || note == 'X'){
        if(isNumber(buffer.charAt(0))){
          uint32_t nv = readNumber(buffer);
          if(nv < 128 && nv >= 0)
            MIDI.sendControlChange(0, nv, currentChannel);
        }
      }else if(note == 'y' || note == 'Y'){
        if(isNumber(buffer.charAt(0))){
          uint32_t nv = readNumber(buffer);
          if(nv < 128 && nv >= 0)
            MIDI.sendControlChange(32, nv, currentChannel);
        }
      }else if(note == 'j' || note == 'J'){
        if(isNumber(buffer.charAt(0))){
          uint32_t nv = readNumber(buffer);
          if(nv < 128 && nv >= 0)
            MIDI.sendControlChange(72, nv, currentChannel);
        }
      }else if(note == 'o' || note == 'O'){
        if(isNumber(buffer.charAt(0))){
          uint32_t nv = readNumber(buffer);
          if(nv < 128 && nv >= 0)
            MIDI.sendControlChange(73, nv, currentChannel);
        }
      }else if(note == 'i' || note == 'I'){
        readInstrument(buffer);
      }
#if ALLOW_MULTI_CHANAL_MIDI
      else if(note == 'k' || note == 'K'){
        if(isNumber(buffer.charAt(0))){
          uint32_t nc = readNumber(buffer);
          currentChannel = nc;
        }
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
      }
    }
  }
}
