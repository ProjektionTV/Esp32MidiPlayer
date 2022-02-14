#ifndef TYPESH
#define TYPESH
#include "main.h"

struct notesBufferEntry{
  uint8_t priority;
  String owner;
  String data;
  uint16_t maxLength;
};

struct instrument{
  String name;
  uint8_t instrument;
  uint8_t bank_MSB;
  uint8_t bank_LSB;
};

struct presetSong{
  String name;
  String data;
};

struct openPlayRequest{
  String data;
  bool allowDev;
  uint32_t timeleft;
};

#endif
