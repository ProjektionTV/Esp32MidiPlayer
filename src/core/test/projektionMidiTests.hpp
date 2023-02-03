#ifndef PROJEKTION_MIDI_TEST_PROJEKTION_MIDI_TESTS_HPP_
#define PROJEKTION_MIDI_TEST_PROJEKTION_MIDI_TESTS_HPP_

#include "projektionMidiTestUtil.hpp"

PROJEKTION_MIDI_TEST_START(projektionMidi_00)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_01)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("A", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(125'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_02)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("p", 16);

    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(125'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_03)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("P", 16);

    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(125'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_04)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("bpm120 A", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(250'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(500'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_05)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-x12 4", 16);

    projektionMidi::test::testMidiOut::expectControl(0, 0, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_06)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-X12 4", 16);

    projektionMidi::test::testMidiOut::expectControl(0, 0, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_07)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-y12 4", 16);

    projektionMidi::test::testMidiOut::expectControl(0, 32, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_08)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-Y12 4", 16);

    projektionMidi::test::testMidiOut::expectControl(0, 32, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_09)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-v12A4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_10)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-V12A4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_11)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-q120A4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(250'000, 0)

    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(500'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_12)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-Q120A4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(250'000, 0)

    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(500'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_13)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-Al4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_14)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-AL4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_15)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-j12 4", 16);

    projektionMidi::test::testMidiOut::expectControl(0, 72, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_16)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-J12 4", 16);

    projektionMidi::test::testMidiOut::expectControl(0, 72, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_17)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-o12 4", 16);

    projektionMidi::test::testMidiOut::expectControl(0, 73, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_18)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-O12 4", 16);

    projektionMidi::test::testMidiOut::expectControl(0, 73, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_19)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-i12 4", 16);

    projektionMidi::test::testMidiOut::expectProgram(0, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_20)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-I12 4", 16);

    projektionMidi::test::testMidiOut::expectProgram(0, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_21)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(2, 1)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-k2A4", 16);

    projektionMidi::test::testMidiOut::expectToggle(1, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_22)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(2, 1)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-K2A4", 16);

    projektionMidi::test::testMidiOut::expectToggle(1, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_23)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(2, 1)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-s4", 16);

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectOff(1);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_24)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(2, 1)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("-S4", 16);

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectOff(1);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_25)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("A4a A", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(125'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)

    PROJEKTION_MIDI_TEST_TICK(375'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(500'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_26)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("A4 mv12A4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(125'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectToggle(0, 57, 12);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)

    PROJEKTION_MIDI_TEST_TICK(375'000, 0)

    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(500'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_27)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("A4 Mv12A4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(125'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectToggle(0, 57, 12);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)

    PROJEKTION_MIDI_TEST_TICK(375'000, 0)

    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(500'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_28)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->addInstrument({ .name = "piano", .program = 0, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "vibes", .program = 11, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "organ", .program = 19, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "guitar", .program = 30, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "brass", .program = 62, .bankLsb = 0, .bankMsb = 0 });
    midi->enqueue("organ A4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    projektionMidi::test::testMidiOut::expectProgram(0, 19);
    projektionMidi::test::testMidiOut::expectControl(0, 0, 0);
    projektionMidi::test::testMidiOut::expectControl(0, 32, 0);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_29)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->addInstrument({ .name = "piano", .program = 0, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "vibes", .program = 11, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "organ", .program = 19, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "guitar", .program = 30, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "brass", .program = 62, .bankLsb = 0, .bankMsb = 0 });
    midi->enqueue("-Aivibes4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    projektionMidi::test::testMidiOut::expectProgram(0, 11);
    projektionMidi::test::testMidiOut::expectControl(0, 0, 0);
    projektionMidi::test::testMidiOut::expectControl(0, 32, 0);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_30)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->addInstrument({ .name = "piano", .program = 0, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "vibes", .program = 11, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "organ", .program = 19, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "guitar", .program = 30, .bankLsb = 0, .bankMsb = 0 });
    midi->addInstrument({ .name = "brass", .program = 62, .bankLsb = 0, .bankMsb = 0 });
    midi->enqueue("-Aivibes4iorgan4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    projektionMidi::test::testMidiOut::expectProgram(0, 11);
    projektionMidi::test::testMidiOut::expectControl(0, 0, 0);
    projektionMidi::test::testMidiOut::expectControl(0, 32, 0);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectProgram(0, 19);
    projektionMidi::test::testMidiOut::expectControl(0, 0, 0);
    projektionMidi::test::testMidiOut::expectControl(0, 32, 0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_31)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("~0", 16);

    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_32)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->addPreset( 0, "A");
    midi->addPreset( 1, "D2");
    midi->enqueue("~0", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(125'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(projektionMidi_33)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(2, 1)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)
    midi->enqueue("A A", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(125'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

TESTS_SUB(projektionMidiTests)
    TEST(testOut, projektionMidi_00)
    TEST(testOut, projektionMidi_01)
    TEST(testOut, projektionMidi_02)
    TEST(testOut, projektionMidi_03)
    TEST(testOut, projektionMidi_04)
    TEST(testOut, projektionMidi_05)
    TEST(testOut, projektionMidi_06)
    TEST(testOut, projektionMidi_07)
    TEST(testOut, projektionMidi_08)
    TEST(testOut, projektionMidi_09)
    TEST(testOut, projektionMidi_10)
    TEST(testOut, projektionMidi_11)
    TEST(testOut, projektionMidi_12)
    TEST(testOut, projektionMidi_13)
    TEST(testOut, projektionMidi_14)
    TEST(testOut, projektionMidi_15)
    TEST(testOut, projektionMidi_16)
    TEST(testOut, projektionMidi_17)
    TEST(testOut, projektionMidi_18)
    TEST(testOut, projektionMidi_19)
    TEST(testOut, projektionMidi_20)
    TEST(testOut, projektionMidi_21)
    TEST(testOut, projektionMidi_22)
    TEST(testOut, projektionMidi_23)
    TEST(testOut, projektionMidi_24)
    TEST(testOut, projektionMidi_25)
    TEST(testOut, projektionMidi_26)
    TEST(testOut, projektionMidi_27)
    TEST(testOut, projektionMidi_28)
    TEST(testOut, projektionMidi_29)
    TEST(testOut, projektionMidi_30)
    TEST(testOut, projektionMidi_31)
    TEST(testOut, projektionMidi_32)
    TEST(testOut, projektionMidi_33)
TEST_SUB_END

#endif /* PROJEKTION_MIDI_TEST_PROJEKTION_MIDI_TESTS_HPP_ */
