#ifndef PROJEKTION_MIDI_TEST_STRING_TEXT_WALKER_TESTS_HPP_
#define PROJEKTION_MIDI_TEST_STRING_TEXT_WALKER_TESTS_HPP_

#include "../stringTextWalker.hpp"
#include "testUtil.hpp"
#include "textWalkerTestUtil.hpp"

#define STRING_WALKER_TEST_IMPL(NAME, TEXT, OTHER, BOOL, TEXT_MATCH) TEST_START(NAME) \
        std::string *text = new std::string(TEXT); \
        projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text); \
        ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testTextWalker::expect(stringWalker, OTHER), BOOL, "Read data from text walker " TEXT_MATCH " match '" OTHER "'") \
        delete stringWalker; \
        delete text; \
    TEST_END
#define STRING_WALKER_TEST(NAME, TEXT) STRING_WALKER_TEST_IMPL(NAME, TEXT, TEXT, true, "didn't")
#define STRING_WALKER_INV_TEST(NAME, TEXT, OTHER) STRING_WALKER_TEST_IMPL(NAME, TEXT, OTHER, false, "did")

STRING_WALKER_TEST(stringTextWalker_00_checkSingleChar, "a")
STRING_WALKER_INV_TEST(stringTextWalker_01_checkWrongChar, "a", "b")
STRING_WALKER_TEST(stringTextWalker_02_checkMultiChar, "azbycx")
STRING_WALKER_INV_TEST(stringTextWalker_03_checkWrongMultiChar, "abcxyz", "ahcxyz")
STRING_WALKER_TEST_IMPL(stringTextWalker_04_checkMoreChar, "abc", "ab", true, "didn't")
STRING_WALKER_TEST_IMPL(stringTextWalker_05_checkLessChar, "abc", "abcd", false, "did")

TEST_START(stringTextWalker_06_checkLoop)
    std::string *text = new std::string("abCdef");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testTextWalker::expectWithJump(stringWalker, "abC", "def"), true, "Read data from text walker with jump didn't match 'abCabCdef'")
    delete stringWalker;
    delete text;
TEST_END

TESTS_SUB(stringTextWalkerTests)
    TEST(testOut, stringTextWalker_00_checkSingleChar)
    TEST(testOut, stringTextWalker_01_checkWrongChar)
    TEST(testOut, stringTextWalker_02_checkMultiChar)
    TEST(testOut, stringTextWalker_03_checkWrongMultiChar)
    TEST(testOut, stringTextWalker_04_checkMoreChar)
    TEST(testOut, stringTextWalker_05_checkLessChar)
    TEST(testOut, stringTextWalker_06_checkLoop)
TEST_SUB_END

#endif /* PROJEKTION_MIDI_TEST_STRING_TEXT_WALKER_TESTS_HPP_ */
