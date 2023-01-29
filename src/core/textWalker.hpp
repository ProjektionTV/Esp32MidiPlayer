#ifndef PROJEKTION_MIDI_TEXT_WALKER_HPP_
#define PROJEKTION_MIDI_TEXT_WALKER_HPP_

#include <cinttypes>
#include <cstdlib>

namespace projektionMidi {
    class textWalker {
        public:
        virtual char next() = 0;
        virtual char peek() = 0;
        virtual void skip(uint16_t a = 1) {
            for(uint16_t i = 0; i < a; i++) next();
        };
        virtual ~textWalker() {};
    }; /* class textWalker */

    struct textWalkerAddressHandle {
        union { void *p = nullptr; size_t n; };
    }; /* struct textWalkerAddressHandle */

    class addressableTextWalker : public textWalker {
        public:
        virtual textWalkerAddressHandle getAddress() = 0;
        virtual void setAddress(textWalkerAddressHandle address) = 0;
        virtual void deleteAddress(textWalkerAddressHandle address) {
            if(address.p != nullptr) free(address.p);
        }
        virtual textWalkerAddressHandle copyAddress(textWalkerAddressHandle address) {
            /* TODO: ? null handling ? */
            auto curr = getAddress();
            setAddress(address);
            auto out = getAddress();
            setAddress(curr);
            deleteAddress(curr);
            return out;
        }
    }; /* class addressableTextWalker */
} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_TEXT_WALKER_HPP_ */
