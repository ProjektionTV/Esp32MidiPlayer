#ifndef PROJEKTION_MIDI_WDH_TEXT_WALKER_HPP_
#define PROJEKTION_MIDI_WDH_TEXT_WALKER_HPP_

#include "textWalker.hpp"

namespace projektionMidi {
    class wdhTextWalker : public addressableTextWalker {
        private:
        struct wdhTextWalkerState {
            textWalkerAddressHandle wdh_start;
            textWalkerAddressHandle wdh_rtn;
            uint16_t wdh_do;
            bool wdh_rdy;
            bool wdh_rtndo;
            bool wdh_rtn_out;
        };
        struct wdhTextWalkerAddress {
            textWalkerAddressHandle i;
            wdhTextWalkerState state;
        };
        addressableTextWalker *text;
        wdhTextWalkerState state;

        public:
        textWalkerAddressHandle getAddress() override;
        void setAddress(textWalkerAddressHandle address) override;
        void deleteAddress(textWalkerAddressHandle address) override;

        char peek() override;
        char next() override;
        wdhTextWalker(addressableTextWalker *text);
        ~wdhTextWalker();
    }; /* class wdhTextWalker */
} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_WDH_TEXT_WALKER_HPP_ */
