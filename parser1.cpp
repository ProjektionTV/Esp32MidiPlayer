#include "parser1.h"

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

  bool inloop = true;
  while ((inloop) && (buffer.length() > 0)){
    if(millis() > timeout)
      return;

    if((lastMqttCheck + 10000) < millis())
      loop();

    if(isNumber(buffer.charAt(0))){
      uint32_t length = readNumber(buffer);
      if(length == 0)
        length = 4;
      midiDelay(vierBeatZeit/length);
      if(buffer.charAt(0) == '.'){
        midiDelay((vierBeatZeit/length) / 2);
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
    if(buffer.length() == 0) {
      if(play || note == 'p' || note == 'P')
        midiDelay(vierBeatZeit/4);
      inloop = false;
    }
  }
}
