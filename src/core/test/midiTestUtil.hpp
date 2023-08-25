#ifndef PROJEKTION_MIDI_TEST_MIDI_TEST_UTIL_HPP_
#define PROJEKTION_MIDI_TEST_MIDI_TEST_UTIL_HPP_

#include <algorithm>
#include <cinttypes>
#include <vector>

#include "../midiHandler.hpp"
#include "testUtil.hpp"

namespace projektionMidi {
    namespace test {
        namespace testMidiOut {
            struct midiEvent {
                // do not make this struct have padding or change projektionMidi::test::testMidiOut::operator<(midiEvent a, midiEvent b)
                uint8_t type;
                uint8_t channel;
                uint8_t a;
                uint8_t b;
            };
            bool operator==(midiEvent a, midiEvent b) {
                return a.type == b.type && a.channel == b.channel && a.a == b.a && a.b == b.b;
            }
            bool operator!=(midiEvent a, midiEvent b) {
                return !(a == b);
            }
            /**
             * @author NOT gkram but known to gkram, so ask gkram the questions: they will be passed on to the author
             */
            bool operator<(midiEvent a, midiEvent b) {
                static_assert(sizeof(midiEvent) == sizeof(uint32_t));
                return *(uint32_t __attribute__((__may_alias__, __aligned__(1))) *)&a < *(uint32_t __attribute__((__may_alias__, __aligned__(1))) *)&b;
            }
            bool operator>(midiEvent a, midiEvent b) {
                return b < a;
            }
            bool operator<=(midiEvent a, midiEvent b) {
                return !(b < a);
            }
            bool operator>=(midiEvent a, midiEvent b) {
                return !(a < b);
            }
            std::vector<midiEvent> expectation;
            std::vector<midiEvent> received;
            void toggle(uint8_t channel, uint8_t note, uint8_t velocity) {
                midiEvent e = { .type = 0, .channel = channel, .a = note, .b = velocity };
                received.emplace_back(e);
            }
            void off(uint8_t channel) {
                midiEvent e = { .type = 1, .channel = channel, .a = 0, .b = 0 };
                received.emplace_back(e);
            }
            void program(uint8_t channel, uint8_t program) {
                midiEvent e = { .type = 2, .channel = channel, .a = program, .b = 0 };
                received.emplace_back(e);
            }
            void control(uint8_t channel, uint8_t control, uint8_t value) {
                midiEvent e = { .type = 3, .channel = channel, .a = control, .b = value };
                received.emplace_back(e);
            }

            void expectToggle(uint8_t channel, uint8_t note, uint8_t velocity) {
                midiEvent e = { .type = 0, .channel = channel, .a = note, .b = velocity };
                expectation.emplace_back(e);
            }
            void expectOff(uint8_t channel) {
                midiEvent e = { .type = 1, .channel = channel, .a = 0, .b = 0 };
                expectation.emplace_back(e);
            }
            void expectProgram(uint8_t channel, uint8_t program) {
                midiEvent e = { .type = 2, .channel = channel, .a = program, .b = 0 };
                expectation.emplace_back(e);
            }
            void expectControl(uint8_t channel, uint8_t control, uint8_t value) {
                midiEvent e = { .type = 3, .channel = channel, .a = control, .b = value };
                expectation.emplace_back(e);
            }

            /**
             * @return 0: as expected 1: more then expected 2: less then expected 3: more/less then expected
             * @author NOT gkram but known to gkram, so ask gkram the questions: they will be passed on to the author
             */
            uint8_t asExpected() {
                // sort lists
                std::sort(received.begin(), received.end());
                std::sort(expectation.begin(), expectation.end());

                auto i = received.begin();
                auto i_end = received.end();
                auto j = expectation.begin();
                auto j_end = expectation.end();
                if(i == i_end)
                    return (int)(j != j_end) << 1;
                else if(j == j_end)
                    return 1;
                while(*i == *j) {
                    ++i, ++j;
                    if(i == i_end)
                        return (int)(j != j_end) << 1;
                    else if(j == j_end)
                        return 1;
                }
                uint8_t result;
                if(*i < *j) {
                    result = 1;
                } else {
                    result = 2;
                    std::swap(i, j);
                    std::swap(i_end, j_end);
                }
                while(true) {
                    while(*i < *j) {
                        ++i;
                        if(i == i_end)
                            return 3;
                    }
                    while(*i == *j) {
                        ++i, ++j;
                        if(i == i_end)
                            return result | (j == j_end ? 0 : 3);
                        else if(j == j_end)
                            return result;
                    }
                    if(*j < *i)
                        return 3;
                }
            }
            /**
             * @author NOT gkram but known to gkram, so ask gkram the questions: they will be passed on to the author
             */
            template<class It, class End>
            class midiDiffFunc {
                public:
                struct it_result { midiEvent e; uint16_t count; };
                private:
                It i;
                End end;
                it_result last = {};
                public:
                midiDiffFunc(It i, End end): i(i), end(end) {}
                it_result operator()() {
                    if(i == end)
                        return it_result{};
                    if(last.count == 0) {
                        last.e = *i;
                        last.count = 1;
                    }
                    uint16_t count = 1;
                    midiEvent e = last.e;
                    while(++i != end) {
                        if(!(*i == e)) {
                            last.e = *i;
                            break;
                        }
                        ++count;
                    }
                    return it_result{e, count};
                }
            };
            void printDiff(out *testOut) {
                midiDiffFunc r(received.begin(), received.end());
                midiDiffFunc e(expectation.begin(), expectation.end());
                auto re = r();
                auto ee = e();
                if(re.count != 0 && ee.count != 0)
                    while(true) {
                        if(re.e == ee.e) {
                            if(re.count < ee.count) {
                                // received less then expected
                                testOut->midiDiff(re.e.type, re.e.channel, re.e.a, re.e.b, ee.count - re.count, true);
                            } else if(ee.count < re.count) {
                                // received more then expected
                                testOut->midiDiff(re.e.type, re.e.channel, re.e.a, re.e.b, re.count - ee.count, false);
                            }
                            re = r();
                            ee = e();
                            if(re.count == 0 || ee.count == 0) break;
                        }
                        if(re.e < ee.e) {
                            // received more then expected
                            testOut->midiDiff(re.e.type, re.e.channel, re.e.a, re.e.b, re.count, false);
                            re = r();
                            if(re.count == 0) break;
                        }
                        if(ee.e < re.e) {
                            // received less then expected
                            testOut->midiDiff(ee.e.type, ee.e.channel, ee.e.a, ee.e.b, ee.count, true);
                            ee = e();
                            if(ee.count == 0) break;
                        }
                    }
                // clear lists since one is empty
                while(re.count) {
                    // received more then expected
                    testOut->midiDiff(re.e.type, re.e.channel, re.e.a, re.e.b, re.count, false);
                    re = r();
                }
                while(ee.count) {
                    // received less then expected
                    testOut->midiDiff(ee.e.type, ee.e.channel, ee.e.a, ee.e.b, ee.count, true);
                    ee = e();
                }
            }
            void clear() {
                expectation.clear();
                received.clear();
            }
        }; /* namespace testMidiOut */
        class midiTestHandler : public midiHandler::midiEventHandler {
            public:
            void note(uint8_t channel, uint8_t note, uint8_t velocity) override {
                testMidiOut::toggle(channel, note, velocity);
            }
            void allOff(uint8_t channel) override {
                testMidiOut::off(channel);
            }
            void programChange(uint8_t channel, uint8_t program) override {
                testMidiOut::program(channel, program);
            }
            void controlChange(uint8_t channel, uint8_t control, uint8_t value) override {
                testMidiOut::control(channel, control, value);
            }
        } midiTestHandler;
    } /* namespace test */
} /* namespace projektionMidi */

TEST_START(midiTestUtil_00)
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testMidiOut::asExpected(), 0, "Midi Output is not as expected!")
TEST_END

TEST_START(midiTestUtil_01_lessThenExpected)
    projektionMidi::test::testMidiOut::expectToggle(0, 63, 127);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testMidiOut::asExpected(), 2, "Midi Output is not as expected!")
    projektionMidi::test::testMidiOut::clear();
TEST_END

TEST_START(midiTestUtil_02_moreThenExpected)
    projektionMidi::test::midiTestHandler.note(0, 63, 127);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testMidiOut::asExpected(), 1, "Midi Output is not as expected!")
    projektionMidi::test::testMidiOut::clear();
TEST_END

TEST_START(midiTestUtil_03_moreLessThenExpected)
    projektionMidi::test::testMidiOut::expectToggle(0, 64, 127);
    projektionMidi::test::midiTestHandler.note(0, 62, 127);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testMidiOut::asExpected(), 3, "Midi Output is not as expected!")
    projektionMidi::test::testMidiOut::clear();
TEST_END

TEST_START(midiTestUtil_04_match)
    projektionMidi::test::testMidiOut::expectToggle(0, 63, 127);
    projektionMidi::test::midiTestHandler.note(0, 63, 127);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testMidiOut::asExpected(), 0, "Midi Output is not as expected!")
    projektionMidi::test::testMidiOut::clear();
TEST_END

TEST_START(midiTestUtil_05_lessThenExpectedWithSort)
    projektionMidi::test::testMidiOut::expectToggle(0, 60, 63);
    projektionMidi::test::midiTestHandler.note(0, 60, 63);
    projektionMidi::test::testMidiOut::expectToggle(0, 63, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 60, 63);
    projektionMidi::test::midiTestHandler.note(0, 60, 63);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testMidiOut::asExpected(), 2, "Midi Output is not as expected!")
    projektionMidi::test::testMidiOut::clear();
TEST_END

TEST_START(midiTestUtil_06_moreThenExpectedWithSort)
    projektionMidi::test::testMidiOut::expectToggle(0, 60, 63);
    projektionMidi::test::midiTestHandler.note(0, 60, 63);
    projektionMidi::test::midiTestHandler.note(0, 63, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 60, 63);
    projektionMidi::test::midiTestHandler.note(0, 60, 63);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testMidiOut::asExpected(), 1, "Midi Output is not as expected!")
    projektionMidi::test::testMidiOut::clear();
TEST_END

TEST_START(midiTestUtil_07_moreLessThenExpectedWithSort)
    projektionMidi::test::testMidiOut::expectToggle(0, 60, 63);
    projektionMidi::test::midiTestHandler.note(0, 60, 63);
    projektionMidi::test::testMidiOut::expectToggle(0, 64, 127);
    projektionMidi::test::midiTestHandler.note(0, 62, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 60, 63);
    projektionMidi::test::midiTestHandler.note(0, 60, 63);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testMidiOut::asExpected(), 3, "Midi Output is not as expected!")
    projektionMidi::test::testMidiOut::clear();
TEST_END

TEST_START(midiTestUtil_08_matchWithSort)
    projektionMidi::test::testMidiOut::expectToggle(0, 60, 63);
    projektionMidi::test::midiTestHandler.note(0, 60, 63);
    projektionMidi::test::testMidiOut::expectToggle(0, 63, 127);
    projektionMidi::test::midiTestHandler.note(0, 63, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 60, 63);
    projektionMidi::test::midiTestHandler.note(0, 60, 63);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testMidiOut::asExpected(), 0, "Midi Output is not as expected!")
    projektionMidi::test::testMidiOut::clear();
TEST_END

TEST_START(midiTestUtil_09_multipleTypes)
    projektionMidi::test::testMidiOut::expectToggle(0, 63, 127);
    projektionMidi::test::testMidiOut::expectProgram(5, 13);
    projektionMidi::test::testMidiOut::expectControl(0, 72, 127);
    projektionMidi::test::testMidiOut::expectOff(5);
    projektionMidi::test::testMidiOut::expectToggle(3, 12, 127);
    projektionMidi::test::midiTestHandler.note(0, 63, 127);
    projektionMidi::test::midiTestHandler.programChange(5, 13);
    projektionMidi::test::midiTestHandler.controlChange(0, 72, 127);
    projektionMidi::test::midiTestHandler.allOff(5);
    projektionMidi::test::midiTestHandler.note(3, 12, 127);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testMidiOut::asExpected(), 0, "Midi Output is not as expected!")
    projektionMidi::test::testMidiOut::clear();
TEST_END

TESTS_SUB(midiTestUtilTests)
    TEST(testOut, midiTestUtil_00)
    TEST(testOut, midiTestUtil_01_lessThenExpected)
    TEST(testOut, midiTestUtil_02_moreThenExpected)
    TEST(testOut, midiTestUtil_03_moreLessThenExpected)
    TEST(testOut, midiTestUtil_04_match)
    TEST(testOut, midiTestUtil_05_lessThenExpectedWithSort)
    TEST(testOut, midiTestUtil_06_moreThenExpectedWithSort)
    TEST(testOut, midiTestUtil_07_moreLessThenExpectedWithSort)
    TEST(testOut, midiTestUtil_08_matchWithSort)
    TEST(testOut, midiTestUtil_09_multipleTypes)
TEST_SUB_END

#endif /* PROJEKTION_MIDI_TEST_MIDI_TEST_UTIL_HPP_ */
