#ifndef PROJEKTION_MIDI_TEST_TEST_UTIL_HPP_
#define PROJEKTION_MIDI_TEST_TEST_UTIL_HPP_

#include <cinttypes>

#define TESTS_TYPE_COUNT uint32_t
#define TESTS_TYPE_OUT projektionMidi::test::out

#define TESTS_START_IMPL TESTS_TYPE_COUNT TEST_success = 0, TEST_ignored = 0, TEST_failed = 0;
#define TESTS_START TESTS_START_IMPL bool TEST_SKIP = false;
#define TEST_IMPL(OUT, TEST) TEST_RESULT_##TEST = TEST_SKIP ? false : TEST_FUNC_##TEST(OUT); if(TEST_RESULT_##TEST) TEST_success++; else TEST_failed++; if(TEST_SKIP) TEST_ignored++;
#define TEST(OUT, TEST) bool TEST_IMPL(OUT, TEST)
#define TEST_IF_PASSED(OUT, TEST, OTHER) bool TEST_RESULT_##TEST = false; if(TEST_RESULT_##OTHER) { TEST_IMPL(OUT, TEST) } else { TEST_ignored++; OUT->reportIgnoredTest(#TEST, #OTHER, 0, false, __FILE__, __LINE__); }
#define TESTS_REPORT_STATS(OUT) OUT->reportTestsResult(TEST_success, TEST_ignored, TEST_failed);
#define TESTS_FAILED ((TEST_ignored | TEST_failed) ? true : false)

#define TESTS_SUB(NAME) bool NAME(TESTS_TYPE_OUT *testOut, TESTS_TYPE_COUNT &TEST_success_, TESTS_TYPE_COUNT &TEST_ignored_, TESTS_TYPE_COUNT &TEST_failed_, bool TEST_SKIP = false) { TESTS_START_IMPL testOut->osYield();
#define TESTS_SUB_CALL_IMPL(OUT, NAME) TEST_SUB_RESULT_##NAME = TEST_SKIP ? false : NAME(OUT, TEST_success, TEST_ignored, TEST_failed); if(TEST_SKIP) TEST_ignored++;
#define TESTS_SUB_CALL(OUT, NAME) bool TESTS_SUB_CALL_IMPL(OUT, NAME)
#define TEST_SUB_END TEST_success_ += TEST_success; TEST_ignored_ += TEST_ignored; TEST_failed_ += TEST_failed; return TEST_SKIP ? false : !TESTS_FAILED; }
#define TEST_IF_SUB_PASSED(OUT, TEST, OTHER) bool TEST_RESULT_##TEST = false; if(TEST_SUB_RESULT_##OTHER) { TEST_IMPL(OUT, TEST) } else { TEST_ignored++; OUT->reportIgnoredTest(#TEST, #OTHER, 1, false, __FILE__, __LINE__); }

#define TEST__IF_(OTHER, TYPE, NAME, DUETYPE, PREFIX, OUT, CODE) if(PREFIX##OTHER) { CODE } else { TEST_ignored++; OUT->reportIgnoredTest(#NAME, #OTHER, DUETYPE, TYPE, __FILE__, __LINE__); }
#define TEST__IF_SUB_RESULT(OTHER, TYPE, NAME, OUT, CODE) TEST__IF_(OTHER, TYPE, NAME, 1, TEST_SUB_RESULT_, OUT, CODE)
#define TEST__IF_RESULT(OTHER, TYPE, NAME, OUT, CODE) TEST__IF_(OTHER, TYPE, NAME, 0, TEST_RESULT_, OUT, CODE)

#define TESTS_SUB_CALL_IF_SUB_PASSED(OUT, NAME, OTHER) bool TEST_SUB_RESULT_##NAME = false; TEST__IF_SUB_RESULT(OTHER, true, NAME, OUT, TESTS_SUB_CALL_IMPL(OUT, NAME))
#define TESTS_SUB_CALL_IF_PASSED(OUT, NAME, OTHER) bool TEST_SUB_RESULT_##NAME = false; TEST__IF_RESULT(OTHER, true, NAME, OUT, TESTS_SUB_CALL_IMPL(OUT, NAME))

#define TEST_START_REPORTLESS(NAME) bool TEST_FUNC_##NAME(TESTS_TYPE_OUT *TEST_OUT) { bool TEST_PASSED = true; const char *TEST_NAME = #NAME; TEST_OUT->osYield();
#define TEST_START(NAME) TEST_START_REPORTLESS(NAME) TEST_OUT->reportTestBegin(TEST_NAME, __FILE__, __LINE__);
#define TEST_EXIT_NO_REPORT return TEST_PASSED;
#define TEST_REPORT TEST_OUT->reportTestResult(TEST_NAME, TEST_PASSED, __FILE__, __LINE__);
#define TEST_EXIT_IMPL { TEST_REPORT TEST_EXIT_NO_REPORT }
#define TEST_SET_SUCCESSFUL TEST_PASSED = true;
#define TEST_SET_UNSUCCESSFUL TEST_PASSED = false;
#define TEST_EXIT_SUCCESSFUL { TEST_SET_SUCCESSFUL TEST_EXIT_IMPL }
#define TEST_EXIT_UNSUCCESSFUL { TEST_SET_UNSUCCESSFUL TEST_EXIT_IMPL }
#define TEST_EXIT_IF_FAILED if(!TEST_PASSED) TEST_EXIT_IMPL
#define TEST_EXIT_IF_PASSED if(TEST_PASSED) TEST_EXIT_IMPL
#define TEST_EXIT_IF_FAILED_SUCCESSFUL if(!TEST_PASSED) TEST_EXIT_SUCCESSFUL
#define TEST_EXIT_IF_PASSED_UNSUCCESSFUL if(TEST_PASSED) TEST_EXIT_UNSUCCESSFUL
#define TEST_END_REPORTLESS TEST_EXIT_NO_REPORT }
#define TEST_END TEST_REPORT TEST_END_REPORTLESS

#define ASSERT_FAIL_REPORT(A, B) TEST_OUT->reportFailedAssert(A, B, __FILE__, __LINE__);
#define ASSERT_FAIL_REPORT_MSG(A, B, MSG) TEST_OUT->reportFailedAssert(A, B, MSG, __FILE__, __LINE__);
#define ASSERT_FAIL_SET_IMPL TEST_PASSED = false;
#define ASSERT_FAIL_IMPL(A, B) { ASSERT_FAIL_SET_IMPL ASSERT_FAIL_REPORT(#A, #B) }
#define ASSERT_FAIL_IMPL_MSG(A, B, MSG) { ASSERT_FAIL_SET_IMPL ASSERT_FAIL_REPORT_MSG(#A, #B, MSG) }
#define ASSERT_FAIL_IMPL_BOOL(A, BOOL) { ASSERT_FAIL_SET_IMPL ASSERT_FAIL_REPORT(#A, BOOL) }

#define ASSERT(A, B) { if((A) != (B)) ASSERT_FAIL_IMPL(A, B) }
#define ASSERT_IF_PASSED(A, B) if(TEST_PASSED) ASSERT(A, B)
#define ASSERT_IF_FAILED(A, B) if(!TEST_PASSED) ASSERT(A, B)
#define ASSERT_IF_PASSED_REPORT_IGNORE(A, B) ASSERT_IF_PASSED(A, B) else TEST_OUT->reportIgnoredAssert(#A, #B, true, __FILE__, __LINE__);
#define ASSERT_IF_FAILED_REPORT_IGNORE(A, B) ASSERT_IF_FAILED(A, B) else TEST_OUT->reportIgnoredAssert(#A, #B, false, __FILE__, __LINE__);

#define ASSERT_NO_REPORT(A, B) { if((A) != (B)) ASSERT_FAIL_SET_IMPL }
#define ASSERT_IF_PASSED_NO_REPORT(A, B) if(TEST_PASSED) ASSERT_NO_REPORT(A, B)
#define ASSERT_IF_FAILED_NO_REPORT(A, B) if(!TEST_PASSED) ASSERT_NO_REPORT(A, B)

#define ASSERT_CUSTOM_MESSAGE(A, B, MSG) { if((A) != (B)) ASSERT_FAIL_IMPL_MSG(A, B, MSG) }
#define ASSERT_CUSTOM_MESSAGE_IF_PASSED(A, B, MSG) if(TEST_PASSED) ASSERT_CUSTOM_MESSAGE(A, B, MSG)
#define ASSERT_CUSTOM_MESSAGE_IF_FAILED(A, B, MSG) if(!TEST_PASSED) ASSERT_CUSTOM_MESSAGE(A, B, MSG)
#define ASSERT_CUSTOM_MESSAGE_IF_PASSED_REPORT_IGNORE(A, B, MSG) ASSERT_CUSTOM_MESSAGE_IF_PASSED(A, B, MSG) else TEST_OUT->reportIgnoredAssert(#A, #B, true, __FILE__, __LINE__);
#define ASSERT_CUSTOM_MESSAGE_IF_FAILED_REPORT_IGNORE(A, B, MSG) ASSERT_CUSTOM_MESSAGE_IF_FAILED(A, B, MSG) else TEST_OUT->reportIgnoredAssert(#A, #B, false, __FILE__, __LINE__);


#define ASSERT_TRUE(A) { if(!(A)) ASSERT_FAIL_IMPL_BOOL(A, true) }
#define ASSERT_TRUE_IF_PASSED(A) if(TEST_PASSED) ASSERT_TRUE(A)
#define ASSERT_TRUE_IF_FAILED(A) if(!TEST_PASSED) ASSERT_TRUE(A)
#define ASSERT_TRUE_IF_PASSED_REPORT_IGNORE(A) ASSERT_TRUE_IF_PASSED(A) else TEST_OUT->reportIgnoredAssert(#A, true, true, __FILE__, __LINE__);
#define ASSERT_TRUE_IF_FAILED_REPORT_IGNORE(A) ASSERT_TRUE_IF_FAILED(A) else TEST_OUT->reportIgnoredAssert(#A, true, false, __FILE__, __LINE__);

#define ASSERT_FALSE(A) { if((A)) ASSERT_FAIL_IMPL_BOOL(A, false) }
#define ASSERT_FALSE_IF_PASSED(A) if(TEST_PASSED) ASSERT_FALSE(A)
#define ASSERT_FALSE_IF_FAILED(A) if(!TEST_PASSED) ASSERT_FALSE(A)
#define ASSERT_FALSE_IF_PASSED_REPORT_IGNORE(A) ASSERT_FALSE_IF_PASSED(A) else TEST_OUT->reportIgnoredAssert(#A, false, true, __FILE__, __LINE__);
#define ASSERT_FALSE_IF_FAILED_REPORT_IGNORE(A) ASSERT_FALSE_IF_FAILED(A) else TEST_OUT->reportIgnoredAssert(#A, false, false, __FILE__, __LINE__);


#define ASSERT_TESTS_START { TESTS_START
#define ASSERT_TEST(TEST_) TEST(TEST_OUT, TEST_)
#define ASSERT_TEST_IF_PASSED(TEST) TEST_IF_PASSED(TEST_OUT, TEST)
#define ASSERT_TESTS_PASSED ASSERT_FALSE(TESTS_FAILED)
#define ASSERT_TESTS_FAILED ASSERT_TRUE(TESTS_FAILED)
#define ASSERT_TESTS_END }

#define ASSERT_TEST_PASSED(TEST_) ASSERT_TESTS_START ASSERT_TEST(TEST_) ASSERT_TESTS_PASSED ASSERT_TESTS_END
#define ASSERT_TEST_FAILED(TEST_) ASSERT_TESTS_START ASSERT_TEST(TEST_) ASSERT_TESTS_FAILED ASSERT_TESTS_END

namespace projektionMidi {
    namespace test {
        struct out {
            virtual void reportTestsResult(TESTS_TYPE_COUNT success = 0, TESTS_TYPE_COUNT ignored = 0, TESTS_TYPE_COUNT failed = 0) = 0;

            virtual void reportIgnoredTest(const char *name, const char *other, uint8_t dueTo, bool isSub, const char *file, int line) = 0;
            virtual void reportTestBegin(const char *name, const char *file, int line) = 0;
            virtual void reportTestResult(const char *name, bool rtn, const char *file, int line) = 0;

            virtual void reportIgnoredAssert(const char *a, const char *b, bool passed, const char *file, int line) = 0;
            virtual void reportIgnoredAssert(const char *a, bool b, bool passed, const char *file, int line) {
                reportIgnoredAssert(a, b ? "true" : "false", passed, file, line);
            }
            virtual void reportFailedAssert(const char *a, const char *b, const char *file, int line) = 0;
            virtual void reportFailedAssert(const char *a, const char *b, const char *msg, const char *file, int line) = 0;
            virtual void reportFailedAssert(const char *a, bool b, const char *file, int line) {
                reportFailedAssert(a, b ? "true" : "false", file, line);
            }

            virtual void info(const char *text) = 0;
            virtual void info(const char singleChar) = 0;
            virtual void warning(const char *text) = 0;
            virtual void error(const char *text) = 0;

            virtual void midiDiff(uint8_t type, uint8_t channel, uint8_t a, uint8_t b, uint16_t count, bool expectation) = 0;

            virtual void osYield() { }
        }; /* struct out */
    } /* namespace test */
} /* namespace projektionMidi */

TEST_START(testFramework_00)
TEST_END

TEST_START(testFramework_01)
    ASSERT(1, 1)
TEST_END

TEST_START(testFramework_02)
    ASSERT_TRUE(true)
TEST_END

TEST_START(testFramework_03)
    ASSERT_FALSE(false)
TEST_END

TEST_START(testFramework_04)
    ASSERT_NO_REPORT(0, 1)
    TEST_EXIT_IF_PASSED_UNSUCCESSFUL
    TEST_SET_SUCCESSFUL
TEST_END

TEST_START(testFramework_05)
    ASSERT_IF_PASSED_NO_REPORT(0, 1)
    TEST_EXIT_IF_PASSED_UNSUCCESSFUL
    TEST_SET_SUCCESSFUL
TEST_END

TEST_START(testFramework_06_0)
TEST_END

TEST_START(testFramework_06)
    ASSERT_TESTS_START
        ASSERT_TEST(testFramework_06_0)
        ASSERT_TESTS_PASSED
    ASSERT_TESTS_END
TEST_END

TEST_START_REPORTLESS(testFramework_07_0)
    ASSERT_NO_REPORT(0, 1)
TEST_END_REPORTLESS

TEST_START(testFramework_07)
    ASSERT_TESTS_START
        ASSERT_TEST(testFramework_07_0)
        ASSERT_TESTS_FAILED
    ASSERT_TESTS_END
TEST_END

TEST_START(testFramework_08_0)
TEST_END

TEST_START(testFramework_08)
    ASSERT_TEST_PASSED(testFramework_08_0)
TEST_END

TEST_START_REPORTLESS(testFramework_09_0)
    ASSERT_NO_REPORT(0, 1)
TEST_END_REPORTLESS

TEST_START(testFramework_09)
    ASSERT_TEST_FAILED(testFramework_09_0)
TEST_END


TESTS_SUB(frameworkTests)
    TEST(testOut, testFramework_00)
    TEST(testOut, testFramework_01)
    TEST(testOut, testFramework_02)
    TEST(testOut, testFramework_03)
    TEST(testOut, testFramework_04)
    TEST(testOut, testFramework_05)
    TEST(testOut, testFramework_06)
    TEST(testOut, testFramework_07)
    TEST(testOut, testFramework_08)
    TEST(testOut, testFramework_09)
TEST_SUB_END

#endif /* PROJEKTION_MIDI_TEST_TEST_UTIL_HPP_ */
