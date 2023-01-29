#ifndef PROJEKTION_MIDI_TEXT_WALKER_UTIL_HPP_
#define PROJEKTION_MIDI_TEXT_WALKER_UTIL_HPP_

#include "textWalker.hpp"

namespace projektionMidi {
    namespace textWalkerUtil {
        uint32_t readUInt32(textWalker *walker);
        void skipAfterSpaces(textWalker *walker);
        bool startsWith(addressableTextWalker *walker, const char *text);
        bool startsWithImpl(textWalker *walker, const char *text);
    } /* namespace textWalkerUtil */
} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_TEXT_WALKER_UTIL_HPP_ */
