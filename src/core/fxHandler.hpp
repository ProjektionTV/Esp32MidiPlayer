#ifndef PROJEKTION_MIDI_FX_HANDLER_HPP_
#define PROJEKTION_MIDI_FX_HANDLER_HPP_

#include <cinttypes>

namespace projektionMidi {
    class FXHandler {
        public:
        virtual void tick(uint64_t us) = 0;
        virtual void setBpm(uint32_t bpm) = 0;
        virtual void start() {};
        virtual void stop() {};
    }; /* class FXHandler */
} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_FX_HANDLER_HPP_ */
