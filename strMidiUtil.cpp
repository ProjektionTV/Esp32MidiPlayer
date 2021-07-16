#include "strMidiUtil.h"

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

uint32_t readNumber(String &s){
  uint32_t curr = 0;
  while(isNumber(s.charAt(0))){
    curr *= 10;
    curr += s.charAt(0) - '0'; // get numerical value of number
    s.remove(0,1);
  }
  return curr;
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

void readHalbton(String &s, bool &halbtonC, bool &halbtonB){
  halbtonC = (s.charAt(0) == '#');
  if(halbtonC)
    s.remove(0,1);
  halbtonB = (s.charAt(0) == 'b');
  if(halbtonB)
    s.remove(0,1);
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

void deleteSpace(String &s){
  while (s.charAt(0) == ' '){
    s.remove(0,1);
  }
}

void midiDelay(uint32_t time){
  if((millis() + time) > timeout){
    mqttLoopedDelay(timeout - millis());
  } else {
    mqttLoopedDelay(time);
  }
}

void mqttLoopedDelay(uint32_t delay){
  uint32_t loopEnd = millis() + delay;
  while (millis() < loopEnd){
    if (!psClient.connected())
      mqttReconnect();
    psClient.loop();
    if(timeout < loopEnd)
      loopEnd = timeout;
  }
}
