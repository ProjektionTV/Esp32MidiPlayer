#ifndef PROJEKTION_MIDI_MIDI_HANDLER_HPP_
#define PROJEKTION_MIDI_MIDI_HANDLER_HPP_

#include <cinttypes>

namespace projektionMidi {
    namespace midiHandler {
        class midiEventHandler {
            public:
            virtual void note(uint8_t channel, uint8_t note, uint8_t velocity) = 0;
            virtual void allOff(uint8_t channel) = 0;
            virtual void programChange(uint8_t channel, uint8_t program) = 0;
            virtual void controlChange(uint8_t channel, uint8_t control, uint8_t value) = 0;
        };
        struct channelMapEntry {
            midiEventHandler *handler;
            uint8_t channel;
        };
    } /* namespace midiHandler */
} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_MIDI_HANDLER_HPP_ */
