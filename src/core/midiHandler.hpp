#ifndef PROJEKTION_MIDI_MIDI_HANDLER_HPP_
#define PROJEKTION_MIDI_MIDI_HANDLER_HPP_

#include <cinttypes>
#include <functional>

namespace projektionMidi {
    namespace midiHandler {
        typedef std::function<void(uint8_t channel, uint8_t note, uint8_t velocity)> noteToggFunc;
        typedef std::function<void(uint8_t channel)> allOffFunc;
        typedef std::function<void(uint8_t channel, uint8_t program)> programFunc;
        typedef std::function<void(uint8_t channel, uint8_t control, uint8_t value)> controlFunc;
        struct eventHandler {
            noteToggFunc note;
            allOffFunc allOff;
            programFunc programChange;
            controlFunc controlChange;
        };
        struct channelMapEntry {
            eventHandler *handler;
            uint8_t channel;
        };

    } /* namespace midiHandler */
} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_MIDI_HANDLER_HPP_ */
