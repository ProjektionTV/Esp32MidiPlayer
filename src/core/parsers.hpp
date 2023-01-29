#ifndef PROJEKTION_MIDI_PASERS_HPP_
#define PROJEKTION_MIDI_PASERS_HPP_

#include "projektionMidi.hpp"

namespace projektionMidi {
    void parser1(projektionMidi *midi, playStack *stack, uint64_t us);
    void parser2(projektionMidi *midi, playStack *stack, uint64_t us);
    uint8_t readNote(char noteChar, textWalker *text);
    float readDelay(char readChar, textWalker *text);
} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_PASERS_HPP_ */
