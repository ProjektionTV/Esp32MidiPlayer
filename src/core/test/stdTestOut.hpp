#ifndef PROJEKTION_MIDI_TEST_STD_TEST_OUT_HPP_
#define PROJEKTION_MIDI_TEST_STD_TEST_OUT_HPP_

#include <cstdio>
#include "testUtil.hpp"

#if PROJEKTION_MIDI_TEST_ANSI
#define PROJEKTION_MIDI_TEST_ANSI_RST       "\x1b[0m"
#define PROJEKTION_MIDI_TEST_ANSI_PASS      "\x1b[32m"
#define PROJEKTION_MIDI_TEST_ANSI_INFO      "\x1b[37m"
#define PROJEKTION_MIDI_TEST_ANSI_WARN      "\x1b[33m"
#define PROJEKTION_MIDI_TEST_ANSI_ERROR     "\x1b[31m"
#define PROJEKTION_MIDI_TEST_ANSI_IGNORE    "\x1b[34m"
#define PROJEKTION_MIDI_TEST_ANSI_MIDI      "\x1b[35m"
#else
#define PROJEKTION_MIDI_TEST_ANSI_RST       ""
#define PROJEKTION_MIDI_TEST_ANSI_PASS      ""
#define PROJEKTION_MIDI_TEST_ANSI_INFO      ""
#define PROJEKTION_MIDI_TEST_ANSI_WARN      ""
#define PROJEKTION_MIDI_TEST_ANSI_ERROR     ""
#define PROJEKTION_MIDI_TEST_ANSI_IGNORE    ""
#define PROJEKTION_MIDI_TEST_ANSI_MIDI      ""
#endif

namespace projektionMidi {
    namespace test {
        struct silentStdTestOut : out {
            void reportTestsResult(TESTS_TYPE_COUNT success = 0, TESTS_TYPE_COUNT ignored = 0, TESTS_TYPE_COUNT failed = 0) {
                TESTS_TYPE_COUNT total = success + ignored + failed;
                if(total == success) std::puts(PROJEKTION_MIDI_TEST_ANSI_PASS "[RESULT] All Tests passed." PROJEKTION_MIDI_TEST_ANSI_RST);
                std::printf(PROJEKTION_MIDI_TEST_ANSI_INFO "[RESULT] %u Tests Passed, %u Tests Failed, %u Tests Ignored\n" PROJEKTION_MIDI_TEST_ANSI_RST, success, failed, ignored);
            }

            void reportIgnoredTest(const char *name, const char *other, uint8_t dueTo, bool isSub, const char *file, int line) { }
            void reportTestBegin(const char *name, const char *file, int line) { }
            void reportTestResult(const char *name, bool rtn, const char *file, int line) {
                if(!rtn)
                    std::printf(PROJEKTION_MIDI_TEST_ANSI_ERROR "[TEST:FAIL] [%s:%i] %s\n" PROJEKTION_MIDI_TEST_ANSI_RST, file, line, name);
            }

            void reportIgnoredAssert(const char *a, const char *b, bool passed, const char *file, int line) { }
            void reportFailedAssert(const char *a, const char *b, const char *file, int line) {
                std::printf(PROJEKTION_MIDI_TEST_ANSI_ERROR "[ASSERT:FAILED] [%s:%i] '%s' is not '%s'.\n" PROJEKTION_MIDI_TEST_ANSI_RST, file, line, a, b);
            }
            void reportFailedAssert(const char *a, const char *b, const char *msg, const char *file, int line) {
                std::printf(PROJEKTION_MIDI_TEST_ANSI_ERROR "[ASSERT:FAILED] [%s:%i] '%s' is not '%s': %s\n" PROJEKTION_MIDI_TEST_ANSI_RST, file, line, a, b, msg);
            }

            void info(const char *text) { }
            void info(const char singleChar) { }
            void warning(const char *text) {
                std::printf(PROJEKTION_MIDI_TEST_ANSI_WARN "[WARNING] %s\n" PROJEKTION_MIDI_TEST_ANSI_RST, text);
            }
            void error(const char *text) {
                std::printf(PROJEKTION_MIDI_TEST_ANSI_ERROR "[ERROR] %s\n" PROJEKTION_MIDI_TEST_ANSI_RST, text);
            }

            void midiDiff(uint8_t type, uint8_t channel, uint8_t a, uint8_t b, uint16_t count, bool expectation) { }
        }; /* struct silentStdTestOut */

        struct stdTestOut : silentStdTestOut {
            void reportIgnoredTest(const char *name, const char *other, uint8_t dueTo, bool isSub, const char *file, int line) {
                // dueTo 0: test 1: sub
                // isSub true: sub false: test
                const char* sub = isSub ? "Sub tests" : "Test";
                const char* due = dueTo ? "sub tests" : "test";

                std::printf(PROJEKTION_MIDI_TEST_ANSI_IGNORE "[IGNORE:TEST] [%s:%i] %s(%s) ignored due to %s(%s) fail.\n" PROJEKTION_MIDI_TEST_ANSI_RST, file, line, sub, name, due, other);
            }
            void reportTestBegin(const char *name, const char *file, int line) {
                std::printf(PROJEKTION_MIDI_TEST_ANSI_INFO "[TEST] [%s:%i] %s\n" PROJEKTION_MIDI_TEST_ANSI_RST, file, line, name);
            }
            void reportTestResult(const char *name, bool rtn, const char *file, int line) {
                if(rtn)
                    std::printf(PROJEKTION_MIDI_TEST_ANSI_PASS "[TEST:SUCCESS] [%s:%i] %s\n" PROJEKTION_MIDI_TEST_ANSI_RST, file, line, name);
                else
                    std::printf(PROJEKTION_MIDI_TEST_ANSI_ERROR "[TEST:FAIL] [%s:%i] %s\n" PROJEKTION_MIDI_TEST_ANSI_RST, file, line, name);
            }

            void reportIgnoredAssert(const char *a, const char *b, bool passed, const char *file, int line) {
                const char *prev = passed ? "passed" : "faild";
                std::printf(PROJEKTION_MIDI_TEST_ANSI_IGNORE "[IGNORE:ASSERT] [%s:%i] (%s == %s) due to previous assersions %s.\n" PROJEKTION_MIDI_TEST_ANSI_RST, file, line, a, b, prev);
            }

            void info(const char *text) {
                std::printf(PROJEKTION_MIDI_TEST_ANSI_INFO "[INFO] %s\n" PROJEKTION_MIDI_TEST_ANSI_RST, text);
            }
            void info(const char singleChar) {
                std::printf(PROJEKTION_MIDI_TEST_ANSI_INFO "[INFO] %c\n" PROJEKTION_MIDI_TEST_ANSI_RST, singleChar);
            }

            void midiDiff(uint8_t type, uint8_t channel, uint8_t a, uint8_t b, uint16_t count, bool expectation) {
                const char *typeT = "unknown";
                if(type == 0) typeT = "toggle";
                if(type == 1) typeT = "off";
                if(type == 2) typeT = "programm";
                if(type == 3) typeT = "controll";
                std::printf(PROJEKTION_MIDI_TEST_ANSI_MIDI "[MIDI] %s%d: %s, channel: %d, a: %d, b: %d\n" PROJEKTION_MIDI_TEST_ANSI_RST, expectation ? "-" : "+", count, typeT, channel, a, b);
            }
        }; /* struct stdTestOut */
    } /* namespace test */
} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_TEST_STD_TEST_OUT_HPP_ */
