#include "song.h"

void playSong(String input, uint32_t timeOutSeconds){

  if(inUserRequest){
    if(amountPlayRequestLeft == MAX_PLAYREQUESTS)
      return;
    playRequests[amountPlayRequestLeft].data = input;
    playRequests[amountPlayRequestLeft].timeleft = timeOutSeconds;
    amountPlayRequestLeft++;
    return;
  }

  inUserRequest = true;

  if(input.startsWith("~")){
    input.remove(0,1);
    if(isNumber(input.charAt(0))){
      uint32_t num = readNumber(input);
      if(num >= 0 && num < AMOUNT_PRESET_SONGS)
        input = presetSongs[num].data;
      else
        input = SONG_NOT_EXISTS;
    }
  }

  Serial.println("PLAY ERKANNT");

  currentChannel = DEFAULT_MIDI_CHANNEL;

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
      bpm = DEFAULT_BPM;
    }
  }else{
    bpm = DEFAULT_BPM;
  }

  while (input.startsWith(" "))
    input.remove(0,1);
    
  readInstrument(input);

  fourBeatTime = (int) ((float) (240000) / ((float) bpm));

  while (input.startsWith(" "))
    input.remove(0,1);

  input = expandLoops(input);

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
  inUserRequest = false;
}
