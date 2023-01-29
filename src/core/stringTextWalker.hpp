#ifndef PROJEKTION_MIDI_STRING_TEXT_WALKER_HPP_
#define PROJEKTION_MIDI_STRING_TEXT_WALKER_HPP_

#include <string>

#include "textWalker.hpp"

namespace projektionMidi {
    class stringTextWalker : public addressableTextWalker {
        private:
        std::string *text;
        std::size_t i;
        bool notAtEnd();
        public:
        void setAddress(textWalkerAddressHandle address) override;
        textWalkerAddressHandle getAddress() override;
        void deleteAddress(textWalkerAddressHandle) override;
        char next() override;
        char peek() override;
        void skip(uint16_t a = 1) override;
        stringTextWalker(std::string *text);
        ~stringTextWalker();
    }; /* class stringTextWalker */
} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_STRING_TEXT_WALKER_HPP_ */
