#ifndef TYPESH
#define TYPESH
#include "main.h"

struct notesBufferEntry{
  uint8_t priority;
  String owner;
  String data;
  uint16_t maxLength;
};

#endif
