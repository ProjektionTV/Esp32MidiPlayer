#ifndef PROJEKTION_MIDI_TEST_TEXT_WALKER_TEST_UTIL_HPP_
#define PROJEKTION_MIDI_TEST_TEXT_WALKER_TEST_UTIL_HPP_

#include "testUtil.hpp"
#include "../textWalker.hpp"

namespace projektionMidi {
    namespace test {
        namespace testTextWalker {
            bool expect(textWalker *walker, const char *text) {
                std::size_t i = 0;
                while(text[i] != '\0')
                    if(walker->peek() == text[i++]) walker->skip();
                    else return false;
                return true;
            }
            /**
             * walker text should be "${textA}${textB}" read will be "${textA}${textA}${textB}"
             * @param walker "hello world!"
             * @param textA "hello "
             * @param textB "world!"
             */
            bool expectWithJump(addressableTextWalker *walker, const char *textA, const char *textB) {
                int success = true;
                auto start = walker->getAddress();
                success &= expect(walker, textA);
                walker->setAddress(start);
                walker->deleteAddress(start);
                success &= expect(walker, textA);
                success &= expect(walker, textB);
                return success;
            }
        }; /* namespace testTextWalker */
    } /* namespace test */
} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_TEST_TEXT_WALKER_TEST_UTIL_HPP_ */
