#ifndef TYPESH
#define TYPESH
#include "main.h"

struct notenBufferEintrag{
  uint8_t priority;
  String besitzer;
  String daten;
  uint16_t maximaleLaenge;
};

struct instrument{
  String name;
  uint8_t instrument;
  uint8_t bank_MSB;
  uint8_t bank_LSB;
};

struct lied{
  String name;
  String daten;
};

#endif
