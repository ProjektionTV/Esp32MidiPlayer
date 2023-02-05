#ifndef PROJEKTION_MIDI_TEST_PROJEKTION_MIDI_TEST_UTIL_HPP_
#define PROJEKTION_MIDI_TEST_PROJEKTION_MIDI_TEST_UTIL_HPP_

#include "midiTestUtil.hpp"
#include "../projektionMidi.hpp"
#include "testUtil.hpp"

TESTS_TYPE_OUT *fout = nullptr;
void sendERR(const char *text) {
    if(fout != nullptr) {
        fout->error(text);
    }
}

#define SET_MIDI_ERR fout = TEST_OUT; midi->setErrorReciever(sendERR);

#define PROJEKTION_MIDI_TEST_START(NAME) TEST_START(NAME) \
        projektionMidi::projektionMidi *midi = new projektionMidi::projektionMidi(); \
        uint8_t rtn = 0;

#define PROJEKTION_MIDI_TEST_END \
        delete midi; \
    TEST_END

#define PROJEKTION_MIDI_TEST_ADD_CHANNEL(TEXT, INTERNAL) \
        midi->addMidiChannel(TEXT, &projektionMidi::test::midiTestHandler, INTERNAL); \
        expectMidiEnd(INTERNAL);

#define PROJEKTION_MIDI_TEST_MATCH_MIDI(EXPECED_STATE) \
        ASSERT((rtn = projektionMidi::test::testMidiOut::asExpected()), EXPECED_STATE) \
        if(rtn != (EXPECED_STATE)) projektionMidi::test::testMidiOut::printDiff(TEST_OUT); \
        projektionMidi::test::testMidiOut::clear();

#define PROJEKTION_MIDI_TEST_TICK(US, EXPECED_STATE) \
        midi->tick(US); \
        PROJEKTION_MIDI_TEST_MATCH_MIDI(EXPECED_STATE)

void expectMidiEnd(uint8_t channel) {
    projektionMidi::test::testMidiOut::expectOff(channel);
    projektionMidi::test::testMidiOut::expectControl(channel, 0, 0);
    projektionMidi::test::testMidiOut::expectControl(channel, 32, 0);
    projektionMidi::test::testMidiOut::expectControl(channel, 72, 63);
    projektionMidi::test::testMidiOut::expectControl(channel, 73, 63);
    projektionMidi::test::testMidiOut::expectControl(channel, 7, 127);
    projektionMidi::test::testMidiOut::expectProgram(channel, 0);
}

#define PROJEKTION_MIDI_TEST_BUFFER(TEXT, USER, TIME, MAX_SIZE, EXPECTED_LENGTH, BO_DELETED, BO_CREATED, BO_MODIFIED, BO_REFUND, BO_ENQUEUEFAILD) { \
        uint32_t cl = 0; \
        ASSERT(midi->bufferOperation(TEXT, USER, TIME, MAX_SIZE, cl), (BO_ENQUEUEFAILD << 6) | (BO_REFUND << 5) | (BO_MODIFIED << 4) | (BO_CREATED << 2) | BO_DELETED) \
        ASSERT(cl, EXPECTED_LENGTH) \
    }

#endif /* PROJEKTION_MIDI_TEST_PROJEKTION_MIDI_TEST_UTIL_HPP_ */
