#ifndef PROJEKTION_MIDI_TEST_BUFFER_TESTS_HPP_
#define PROJEKTION_MIDI_TEST_BUFFER_TESTS_HPP_

#include "projektionMidiTestUtil.hpp"

PROJEKTION_MIDI_TEST_START(buffer_00_passthrough)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    PROJEKTION_MIDI_TEST_BUFFER("A", "test0", 16, 50, 0, 0, 0, 0, 0, 0)

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(buffer_01_passthrough)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    PROJEKTION_MIDI_TEST_BUFFER(";A", "test0", 16, 50, 0, 0, 0, 0, 0, 0)

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(buffer_02_delno)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    PROJEKTION_MIDI_TEST_BUFFER(";l", "test0", 16, 50, 0, 2, 0, 0, 0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(buffer_03_delNoPassthrough)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    PROJEKTION_MIDI_TEST_BUFFER(";lA", "test0", 16, 50, 0, 2, 0, 0, 0, 0)

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(buffer_04_mulCreate)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    PROJEKTION_MIDI_TEST_BUFFER(";ln", "test0", 16, 50, 1, 2, 1, 1, 1, 0)
    PROJEKTION_MIDI_TEST_TICK(0, 0)
    PROJEKTION_MIDI_TEST_BUFFER(";n", "test0", 16, 50, 2, 0, 2, 1, 1, 0)
    PROJEKTION_MIDI_TEST_TICK(0, 0)
    PROJEKTION_MIDI_TEST_BUFFER("A", "test0", 16, 50, 0, 0, 0, 0, 0, 0)

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(buffer_05_createDel)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    PROJEKTION_MIDI_TEST_BUFFER(";nB", "test0", 16, 50, 2, 0, 1, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER(";lA", "test0", 16, 50, 0, 1, 0, 0, 0, 0)

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(buffer_06_append)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    PROJEKTION_MIDI_TEST_BUFFER(";n", "test0", 16, 50, 1, 0, 1, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER(";A", "test0", 16, 50, 3, 0, 0, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER("", "test0", 16, 50, 0, 0, 0, 0, 0, 0)

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(buffer_07_appendCut)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    PROJEKTION_MIDI_TEST_BUFFER(";n", "test0", 16, 2, 1, 0, 1, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER(";AH", "test0", 16, 2, 2, 0, 0, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER("", "test0", 16, 50, 0, 0, 0, 0, 0, 0)

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(buffer_08_multibuffer)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    PROJEKTION_MIDI_TEST_BUFFER(";n A", "test0", 16, 50, 3, 0, 1, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER(";n A H C", "test1", 16, 50, 7, 0, 1, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER("", "test0", 16, 50, 0, 0, 0, 0, 0, 0)

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(buffer_09_deleteAndCreate)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    PROJEKTION_MIDI_TEST_BUFFER(";nB", "test0", 16, 50, 2, 0, 1, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER(";lnA", "test0", 16, 50, 2, 1, 1, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER("", "test0", 16, 50, 0, 0, 0, 0, 0, 0)

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

TESTS_SUB(bufferTests)
    TEST(testOut, buffer_00_passthrough)
    TEST(testOut, buffer_01_passthrough)
    TEST(testOut, buffer_02_delno)
    TEST(testOut, buffer_03_delNoPassthrough)
    TEST(testOut, buffer_04_mulCreate)
    TEST(testOut, buffer_05_createDel)
    TEST(testOut, buffer_06_append)
    TEST(testOut, buffer_07_appendCut)
    TEST(testOut, buffer_08_multibuffer)
    TEST(testOut, buffer_09_deleteAndCreate)
TEST_SUB_END

#endif /* PROJEKTION_MIDI_TEST_BUFFER_TESTS_HPP_ */
