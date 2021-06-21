#ifndef STRMIDIUH
#define STRMIDIUH
#include "main.h"

bool isNumber(char c);
uint32_t readNumber(String &s);
uint8_t readOktaveOffset(String &s);
void readHalbton(String &s, bool &halbtonC, bool &halbtonB);
uint16_t getNoteID(char note, bool &allowHabtonC, bool &allowHabtonB, bool &noteDown);
uint16_t convertNote(uint16_t noteId, uint8_t oktavenOffset, bool habtonC, bool habtonB, bool allowHabtonC, bool allowHabtonB, bool noteDown);
void deleteSpace(String &s);

void midiDelay(uint32_t time);

#endif
