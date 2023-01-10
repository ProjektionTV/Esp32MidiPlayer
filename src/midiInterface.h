#ifndef MIDIIH
#define MIDIIH
#include "main.h"

void playMIDINote(byte channel, byte note, byte velocity);
void playNote(uint16_t note, uint8_t length);
void parser2note(uint16_t note);
void parser2allOFF();

#endif
