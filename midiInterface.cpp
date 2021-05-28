#include "midiInterface.h"

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
