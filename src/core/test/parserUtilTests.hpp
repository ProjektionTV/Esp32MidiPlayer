#ifndef PROJEKTION_MIDI_TEST_PARSER_UTIL_TESTS_HPP_
#define PROJEKTION_MIDI_TEST_PARSER_UTIL_TESTS_HPP_

#include "../parsers.hpp"
#include "../stringTextWalker.hpp"
#include "testUtil.hpp"

// readNote tests
#define NOTE_TEST(TEXT, VALUE) { \
        std::string *text = new std::string(TEXT); \
        projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text); \
        ASSERT(projektionMidi::readNote(stringWalker->next(), stringWalker), VALUE) \
        delete stringWalker; \
        delete text; \
    }

#define OKTAVE_TEST(NUM, MIDDEL, HIGH, BASE, C, D, E, F, G, A, H) TEST_START(parserUtil_readNote_00_oktave##NUM##HIGH) \
        NOTE_TEST(C MIDDEL    , BASE +  0) \
        NOTE_TEST(C MIDDEL "#", BASE +  1) \
        NOTE_TEST(D MIDDEL "b", BASE +  1) \
        NOTE_TEST(D MIDDEL    , BASE +  2) \
        NOTE_TEST(D MIDDEL "#", BASE +  3) \
        NOTE_TEST(E MIDDEL "b", BASE +  3) \
        NOTE_TEST(E MIDDEL    , BASE +  4) \
        NOTE_TEST(F MIDDEL    , BASE +  5) \
        NOTE_TEST(F MIDDEL "#", BASE +  6) \
        NOTE_TEST(G MIDDEL "b", BASE +  6) \
        NOTE_TEST(G MIDDEL    , BASE +  7) \
        NOTE_TEST(G MIDDEL "#", BASE +  8) \
        NOTE_TEST(A MIDDEL "b", BASE +  8) \
        NOTE_TEST(A MIDDEL    , BASE +  9) \
        NOTE_TEST(A MIDDEL "#", BASE + 10) \
        NOTE_TEST(H MIDDEL "b", BASE + 10) \
        NOTE_TEST(H MIDDEL    , BASE + 11) \
    TEST_END

OKTAVE_TEST(3, "'''", UPPER, 12, "C", "D", "E", "F", "G", "A", "H")
OKTAVE_TEST(2, "''" , UPPER, 24, "C", "D", "E", "F", "G", "A", "H")
OKTAVE_TEST(1, "'"  , UPPER, 36, "C", "D", "E", "F", "G", "A", "H")
OKTAVE_TEST(0, ""   , UPPER, 48, "C", "D", "E", "F", "G", "A", "H")
OKTAVE_TEST(0, ""   , LOWER, 60, "c", "d", "e", "f", "g", "a", "h")
OKTAVE_TEST(1, "'"  , LOWER, 72, "c", "d", "e", "f", "g", "a", "h")
OKTAVE_TEST(2, "''" , LOWER, 84, "c", "d", "e", "f", "g", "a", "h")
OKTAVE_TEST(3, "'''", LOWER, 96, "c", "d", "e", "f", "g", "a", "h")

TEST_START(parserUtil_readNote_01_InvalidChar)
    std::string *text = new std::string(" ");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT(projektionMidi::readNote(stringWalker->next(), stringWalker), 128)
    delete stringWalker;
    delete text;
TEST_END

TEST_START(parserUtil_readNote_02_InvalidChar)
    std::string *text = new std::string("|");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT(projektionMidi::readNote(stringWalker->next(), stringWalker), 128)
    delete stringWalker;
    delete text;
TEST_END

TEST_START(parserUtil_readNote_03_PauseChar)
    std::string *text = new std::string("p");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT(projektionMidi::readNote(stringWalker->next(), stringWalker), 128)
    delete stringWalker;
    delete text;
TEST_END

// readDelay tests

TEST_START(parserUtil_readDelay_00_invalid)
    std::string *text = new std::string(" ");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT(projektionMidi::readDelay(stringWalker->next(), stringWalker), 0.0f)
    delete stringWalker;
    delete text;
TEST_END

TEST_START(parserUtil_readDelay_01_wholeNote)
    std::string *text = new std::string("1");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT(projektionMidi::readDelay(stringWalker->peek(), stringWalker), 1.0f)
    delete stringWalker;
    delete text;
TEST_END

TEST_START(parserUtil_readDelay_02_halfNote)
    std::string *text = new std::string("2");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT(projektionMidi::readDelay(stringWalker->peek(), stringWalker), 2.0f)
    delete stringWalker;
    delete text;
TEST_END

TEST_START(parserUtil_readDelay_03_halfNoteDot)
    std::string *text = new std::string("2.");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT(projektionMidi::readDelay(stringWalker->peek(), stringWalker), 2.0f / 1.5f)
    delete stringWalker;
    delete text;
TEST_END

TEST_START(parserUtil_readDelay_04_multiChar)
    std::string *text = new std::string("16");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT(projektionMidi::readDelay(stringWalker->peek(), stringWalker), 16.0f)
    delete stringWalker;
    delete text;
TEST_END

TESTS_SUB(parserUtilTests)
    // test readNote
    TEST(testOut, parserUtil_readNote_00_oktave3UPPER)
    TEST(testOut, parserUtil_readNote_00_oktave2UPPER)
    TEST(testOut, parserUtil_readNote_00_oktave1UPPER)
    TEST(testOut, parserUtil_readNote_00_oktave0UPPER)
    TEST(testOut, parserUtil_readNote_00_oktave0LOWER)
    TEST(testOut, parserUtil_readNote_00_oktave1LOWER)
    TEST(testOut, parserUtil_readNote_00_oktave2LOWER)
    TEST(testOut, parserUtil_readNote_00_oktave3LOWER)
    TEST(testOut, parserUtil_readNote_01_InvalidChar)
    TEST(testOut, parserUtil_readNote_02_InvalidChar)
    TEST(testOut, parserUtil_readNote_03_PauseChar)

    // test readDelay
    TEST(testOut, parserUtil_readDelay_00_invalid)
    TEST(testOut, parserUtil_readDelay_01_wholeNote)
    TEST(testOut, parserUtil_readDelay_02_halfNote)
    TEST(testOut, parserUtil_readDelay_03_halfNoteDot)
    TEST(testOut, parserUtil_readDelay_04_multiChar)
TEST_SUB_END

#endif /* PROJEKTION_MIDI_TEST_PARSER_UTIL_TESTS_HPP_ */
