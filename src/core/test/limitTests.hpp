#ifndef PROJEKTION_MIDI_TEST_LIMIT_TESTS_HPP_
#define PROJEKTION_MIDI_TEST_LIMIT_TESTS_HPP_

#include "projektionMidiTestUtil.hpp"

PROJEKTION_MIDI_TEST_START(limit_00_queue)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->getSettings()->maxQueueSize = 1;
    ASSERT_TRUE(midi->enqueue("a", 16))
    ASSERT_FALSE(midi->enqueue("b", 16))
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(limit_01_bufferQueuePassthrough)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->getSettings()->maxQueueSize = 1;
    PROJEKTION_MIDI_TEST_BUFFER("A", "test0", 16, 50, 0, 0, 0, 0, 0, 0)
    PROJEKTION_MIDI_TEST_BUFFER("B", "test0", 16, 50, 0, 0, 0, 0, 1, 1)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(limit_02_bufferOpQueuePassthrough)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->getSettings()->maxQueueSize = 1;
    PROJEKTION_MIDI_TEST_BUFFER("A", "test0", 16, 50, 0, 0, 0, 0, 0, 0)
    PROJEKTION_MIDI_TEST_BUFFER(";B", "test0", 16, 50, 0, 0, 0, 0, 1, 1)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(limit_03_bufferPlayQueuePassthrough)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->getSettings()->maxQueueSize = 1;
    PROJEKTION_MIDI_TEST_BUFFER("A", "test0", 16, 50, 0, 0, 0, 0, 0, 0)
    PROJEKTION_MIDI_TEST_BUFFER(";n A", "test0", 16, 50, 3, 0, 1, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER("B", "test0", 16, 50, 0, 0, 0, 0, 1, 1)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(limit_04_buffer)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->getSettings()->maxBuffers = 1;
    PROJEKTION_MIDI_TEST_BUFFER(";n A", "test0", 16, 50, 3, 0, 1, 1, 1, 0)
    PROJEKTION_MIDI_TEST_BUFFER(";n B", "test1", 16, 50, 0, 0, 3, 0, 1, 0)
PROJEKTION_MIDI_TEST_END

PROJEKTION_MIDI_TEST_START(limit_05_token)
    PROJEKTION_MIDI_TEST_ADD_CHANNEL(1, 0)
    PROJEKTION_MIDI_TEST_MATCH_MIDI(0)

    midi->getSettings()->maxTokensToParse = 3;
    midi->enqueue("mmmmm", 16);

    PROJEKTION_MIDI_TEST_TICK(0, 0)

    expectMidiEnd(0);
    PROJEKTION_MIDI_TEST_TICK(0, 0)
PROJEKTION_MIDI_TEST_END

TESTS_SUB(limitTests)
    TEST(testOut, limit_00_queue)
    TEST(testOut, limit_01_bufferQueuePassthrough)
    TEST(testOut, limit_02_bufferOpQueuePassthrough)
    TEST(testOut, limit_03_bufferPlayQueuePassthrough)
    TEST(testOut, limit_04_buffer)
    TEST(testOut, limit_05_token)
TEST_SUB_END

#endif /* PROJEKTION_MIDI_TEST_LIMIT_TESTS_HPP_ */
