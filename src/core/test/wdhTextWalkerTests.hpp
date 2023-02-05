#ifndef PROJEKTION_MIDI_TEST_WDH_TEXT_WALKER_TESTS_HPP_
#define PROJEKTION_MIDI_TEST_WDH_TEXT_WALKER_TESTS_HPP_

#include "../stringTextWalker.hpp"
#include "testUtil.hpp"
#include "textWalkerTestUtil.hpp"
#include "../wdhTextWalker.hpp"

#define WDH_WALKER_TEST(NAME, TEXT, EXPANDED) TEST_START(NAME) \
        std::string *text = new std::string(TEXT); \
        projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text); \
        projektionMidi::addressableTextWalker *wdhWalker = new projektionMidi::wdhTextWalker(stringWalker); \
        ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testTextWalker::expect(wdhWalker, EXPANDED), true, "Read data from wdh walker didn't match '" EXPANDED "'") \
        delete wdhWalker; \
        delete stringWalker; \
        delete text; \
    TEST_END

WDH_WALKER_TEST(wdhTextWalker_00_checkSingleChar, "a", "a")
WDH_WALKER_TEST(wdhTextWalker_01_checkMultiChar, "ahC", "ahC")

TEST_START(wdhTextWalker_02_checkJump)
    std::string *text = new std::string("abCdef");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    projektionMidi::addressableTextWalker *wdhWalker = new projektionMidi::wdhTextWalker(stringWalker);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::test::testTextWalker::expectWithJump(wdhWalker, "abC", "def"), true, "Read data from wdh walker with jump didn't match 'abCabCdef'")
    delete wdhWalker;
    delete stringWalker;
    delete text;
TEST_END

WDH_WALKER_TEST(wdhTextWalker_03_simpleLoop, "wa w", "a a ")
WDH_WALKER_TEST(wdhTextWalker_04_loopWithSimpleN, "wa nw", "a a ")
WDH_WALKER_TEST(wdhTextWalker_05_loopWithN, "wa nCw", "a Ca ")
WDH_WALKER_TEST(wdhTextWalker_06_loopWithSimpleU, "wa uw", "a a a ")
WDH_WALKER_TEST(wdhTextWalker_07_loopWithU, "wa uCw", "a a Ca ")
WDH_WALKER_TEST(wdhTextWalker_08_loopWithNU, "wa nDuCw", "a Da Ca ")
WDH_WALKER_TEST(wdhTextWalker_09_loopWithN2U, "wa nDuCuH'w", "a Da Ca H'a ")
WDH_WALKER_TEST(wdhTextWalker_10_loopWithSimpleN2U, "wa nuuw", "a a a a ")

WDH_WALKER_TEST(wdhTextWalker_11_simpleLoop_textAfter, "wa wc", "a a c")
WDH_WALKER_TEST(wdhTextWalker_12_loopWithSimpleN_textAfter, "wa nwc", "a a c")
WDH_WALKER_TEST(wdhTextWalker_13_loopWithN_textAfter, "wa nCwc", "a Ca c")
WDH_WALKER_TEST(wdhTextWalker_14_loopWithSimpleU_textAfter, "wa uwc", "a a a c")
WDH_WALKER_TEST(wdhTextWalker_15_loopWithU_textAfter, "wa uCwg", "a a Ca g")
WDH_WALKER_TEST(wdhTextWalker_16_loopWithNU_textAfter, "wa nDuCwF", "a Da Ca F")
WDH_WALKER_TEST(wdhTextWalker_17_loopWithN2U_textAfter, "wa nDuCuH'wE", "a Da Ca H'a E")
WDH_WALKER_TEST(wdhTextWalker_18_loopWithSimpleN2U_textAfter, "wa nuuwc", "a a a a c")

WDH_WALKER_TEST(wdhTextWalker_19_simpleLoop_loopAfter, "wa wcwe w", "a a ce e ")
WDH_WALKER_TEST(wdhTextWalker_20_loopWithSimpleN_loopAfter, "wa nwcwe w", "a a ce e ")
WDH_WALKER_TEST(wdhTextWalker_21_loopWithN_loopAfter, "wa nCwcwe w", "a Ca ce e ")
WDH_WALKER_TEST(wdhTextWalker_22_loopWithSimpleU_loopAfter, "wa uwcwe w", "a a a ce e ")
WDH_WALKER_TEST(wdhTextWalker_23_loopWithU_loopAfter, "wa uCwgwe w", "a a Ca ge e ")
WDH_WALKER_TEST(wdhTextWalker_24_loopWithNU_loopAfter, "wa nDuCwFwe w", "a Da Ca Fe e ")
WDH_WALKER_TEST(wdhTextWalker_25_loopWithN2U_loopAfter, "wa nDuCuH'wEwe w", "a Da Ca H'a Ee e ")
WDH_WALKER_TEST(wdhTextWalker_26_loopWithSimpleN2U_loopAfter, "wa nuuwcwe w", "a a a a ce e ")

WDH_WALKER_TEST(wdhTextWalker_27_loopWithUCount, "wa nu-1w", "a a a ")
WDH_WALKER_TEST(wdhTextWalker_28_loopWithUCount, "wa nu-2w", "a a a a ")
WDH_WALKER_TEST(wdhTextWalker_29_loopWithUCount, "wa nu-3w", "a a a a a ")

TESTS_SUB(wdhTextWalkerTests)
    TEST(testOut, wdhTextWalker_00_checkSingleChar)
    TEST(testOut, wdhTextWalker_01_checkMultiChar)
    TEST(testOut, wdhTextWalker_02_checkJump)
    TEST(testOut, wdhTextWalker_03_simpleLoop)
    TEST(testOut, wdhTextWalker_04_loopWithSimpleN)
    TEST(testOut, wdhTextWalker_05_loopWithN)
    TEST(testOut, wdhTextWalker_06_loopWithSimpleU)
    TEST(testOut, wdhTextWalker_07_loopWithU)
    TEST(testOut, wdhTextWalker_08_loopWithNU)
    TEST(testOut, wdhTextWalker_09_loopWithN2U)
    TEST(testOut, wdhTextWalker_10_loopWithSimpleN2U)

    TEST(testOut, wdhTextWalker_11_simpleLoop_textAfter)
    TEST(testOut, wdhTextWalker_12_loopWithSimpleN_textAfter)
    TEST(testOut, wdhTextWalker_13_loopWithN_textAfter)
    TEST(testOut, wdhTextWalker_14_loopWithSimpleU_textAfter)
    TEST(testOut, wdhTextWalker_15_loopWithU_textAfter)
    TEST(testOut, wdhTextWalker_16_loopWithNU_textAfter)
    TEST(testOut, wdhTextWalker_17_loopWithN2U_textAfter)
    TEST(testOut, wdhTextWalker_18_loopWithSimpleN2U_textAfter)
    
    TEST(testOut, wdhTextWalker_19_simpleLoop_loopAfter)
    TEST(testOut, wdhTextWalker_20_loopWithSimpleN_loopAfter)
    TEST(testOut, wdhTextWalker_21_loopWithN_loopAfter)
    TEST(testOut, wdhTextWalker_22_loopWithSimpleU_loopAfter)
    TEST(testOut, wdhTextWalker_23_loopWithU_loopAfter)
    TEST(testOut, wdhTextWalker_24_loopWithNU_loopAfter)
    TEST(testOut, wdhTextWalker_25_loopWithN2U_loopAfter)
    TEST(testOut, wdhTextWalker_26_loopWithSimpleN2U_loopAfter)

    TEST(testOut, wdhTextWalker_27_loopWithUCount)
    TEST(testOut, wdhTextWalker_28_loopWithUCount)
    TEST(testOut, wdhTextWalker_29_loopWithUCount)
TEST_SUB_END

#endif /* PROJEKTION_MIDI_TEST_WDH_TEXT_WALKER_TESTS_HPP_ */
