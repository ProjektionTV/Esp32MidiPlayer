#ifndef PROJEKTION_MIDI_TEST_SEGMENTATION_TESTS_HPP_
#define PROJEKTION_MIDI_TEST_SEGMENTATION_TESTS_HPP_

#include "projektionMidiTestUtil.hpp"

PROJEKTION_MIDI_TEST_START(segemntation_00_1xParallel)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->enqueue("A:!H", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(segemntation_01_2xParalel)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->enqueue("A:H", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 59, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(segemntation_02_diffMode)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->enqueue("A:-H4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 59, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(segemntation_03_diffVelocity)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->enqueue("A:-v12H4", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 59, 12);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(segemntation_04_diffVelocity)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->enqueue("-v12A4:-H", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 12);
    projektionMidi::test::testMidiOut::expectToggle(0, 59, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(segemntation_05_diffChannel)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(2, 1)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->enqueue("-k2A4:-H", 16);

    projektionMidi::test::testMidiOut::expectToggle(1, 57, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 59, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    expectMidiEnd(1);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(segemntation_06_bpm)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->enqueue("-q120A4:-H", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 59, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    PROJEKTION_MIDI_TEST_TICK(250'000, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(500'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(segemntation_07_2xSpawnedParallel)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->enqueue("-t1A4:!-H", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 59, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(segemntation_08_3xSpawnedParallel)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->enqueue("-t1A4:-H", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 59, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 59, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(segemntation_09_subTrack)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->enqueue("-T1A4:!-H", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 59, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)

    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(500'000, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(segemntation_10_parallelTrackLimit)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->getSettings()->maxTracks = 2;
    midi->enqueue("A:H:A#", 16);

    projektionMidi::test::testMidiOut::expectToggle(0, 57, 127);
    projektionMidi::test::testMidiOut::expectToggle(0, 59, 127);
    PROJEKTION_MIDI_TEST_TICK(0, 0)

    projektionMidi::test::testMidiOut::expectOff(0);
    projektionMidi::test::testMidiOut::expectOff(0);
    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(250'000, 0)
PROJEKTION_MIDI_TEST_END

TESTS_SUB(segmantationTests)
    TEST(testOut, segemntation_00_1xParallel)
    TEST(testOut, segemntation_01_2xParalel)
    TEST(testOut, segemntation_02_diffMode)
    TEST(testOut, segemntation_03_diffVelocity)
    TEST(testOut, segemntation_04_diffVelocity)
    TEST(testOut, segemntation_05_diffChannel)
    TEST(testOut, segemntation_06_bpm)
    TEST(testOut, segemntation_07_2xSpawnedParallel)
    TEST(testOut, segemntation_08_3xSpawnedParallel)
    TEST(testOut, segemntation_09_subTrack)
    TEST(testOut, segemntation_10_parallelTrackLimit)
TEST_SUB_END

#endif /* PROJEKTION_MIDI_TEST_SEGMENTATION_TESTS_HPP_ */
