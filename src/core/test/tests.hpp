#ifndef PROJEKTION_MIDI_TEST_TESTS_HPP_
#define PROJEKTION_MIDI_TEST_TESTS_HPP_

#include "bufferTests.hpp"
#include "limitTests.hpp"
#include "midiTestUtil.hpp"
#include "parserUtilTests.hpp"
#include "projektionMidiTests.hpp"
#include "stringTextWalkerTests.hpp"
#include "testUtil.hpp"
#include "textWalkerUtilTests.hpp"
#include "wdhTextWalkerTests.hpp"

bool faild = true;

TEST_START_REPORTLESS(allForProjektionMidi)
    ASSERT_NO_REPORT(faild, false);
TEST_END_REPORTLESS

bool test(TESTS_TYPE_OUT *testOut) {
    TESTS_START
    TESTS_SUB_CALL(testOut, frameworkTests)
    TESTS_SUB_CALL_IF_SUB_PASSED(testOut, midiTestUtilTests, frameworkTests)
    TESTS_SUB_CALL_IF_SUB_PASSED(testOut, stringTextWalkerTests, frameworkTests)
    TESTS_SUB_CALL_IF_SUB_PASSED(testOut, wdhTextWalkerTests, stringTextWalkerTests)
    TESTS_SUB_CALL_IF_SUB_PASSED(testOut, textWalkerUtilTests, stringTextWalkerTests)
    TESTS_SUB_CALL_IF_SUB_PASSED(testOut, parserUtilTests, stringTextWalkerTests)
    faild = TESTS_FAILED;
    TEST(testOut, allForProjektionMidi) // TODO: Write this better
    if(TESTS_FAILED) TEST_failed--;
    else TEST_success--;
    TESTS_SUB_CALL_IF_PASSED(testOut, projektionMidiTests, allForProjektionMidi)
    TESTS_SUB_CALL_IF_SUB_PASSED(testOut, bufferTests, projektionMidiTests)
    TESTS_SUB_CALL_IF_SUB_PASSED(testOut, limitTests, bufferTests)
    TESTS_REPORT_STATS(testOut)
    return TESTS_FAILED;
}

#endif /* PROJEKTION_MIDI_TEST_TESTS_HPP_ */
