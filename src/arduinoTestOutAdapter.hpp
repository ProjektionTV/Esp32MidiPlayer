#ifndef ARDUINO_TEST_OUT_ADAPTER_HPP_
#define ARDUINO_TEST_OUT_ADAPTER_HPP_

#include <Arduino.h>
#include "core/test/testUtil.hpp"

struct silentAdruinoTestOut : projektionMidi::test::out {
    void reportTestsResult(TESTS_TYPE_COUNT success = 0, TESTS_TYPE_COUNT ignored = 0, TESTS_TYPE_COUNT failed = 0) {
        TESTS_TYPE_COUNT total = success + ignored + failed;
        if(total == success) Serial.println("[RESULT] All Tests passed.");
        Serial.printf("[RESULT] %u Tests Passed, %u Tests Failed, %u Tests Ignored\n", success, failed, ignored);
    }

    void reportIgnoredTest(const char *name, const char *other, uint8_t dueTo, bool isSub, const char *file, int line) { }
    void reportTestBegin(const char *name, const char *file, int line) { }
    void reportTestResult(const char *name, bool rtn, const char *file, int line) {
        if(!rtn)
            Serial.printf("[TEST:FAIL] [%s:%i] %s\n", file, line, name);
    }

    void reportIgnoredAssert(const char *a, const char *b, bool passed, const char *file, int line) { }
    void reportFailedAssert(const char *a, const char *b, const char *file, int line) {
        Serial.printf("[ASSERT:FAILED] [%s:%i] '%s' is not '%s'.\n", file, line, a, b);
    }
    void reportFailedAssert(const char *a, const char *b, const char *msg, const char *file, int line) {
        Serial.printf("[ASSERT:FAILED] [%s:%i] '%s' is not '%s': %s\n", file, line, a, b, msg);
    }

    void info(const char *text) { }
    void info(const char singleChar) { }
    void warning(const char *text) {
        Serial.printf("[WARNING] %s\n", text);
    }
    void error(const char *text) {
        Serial.printf("[ERROR] %s\n", text);
    }

    void midiDiff(uint8_t type, uint8_t channel, uint8_t a, uint8_t b, uint16_t count, bool expectation) { }

    void osYield() {
        yield();
    }
};

struct adruinoTestOut : silentAdruinoTestOut {
    void reportIgnoredTest(const char *name, const char *other, uint8_t dueTo, bool isSub, const char *file, int line) {
        // dueTo 0: test 1: sub
        // isSub true: sub false: test
        const char* sub = isSub ? "Sub tests" : "Test";
        const char* due = dueTo ? "sub tests" : "test";

        Serial.printf("[IGNORE:TEST] [%s:%i] %s(%s) ignored due to %s(%s) fail.\n", file, line, sub, name, due, other);
    }
    void reportTestBegin(const char *name, const char *file, int line) {
        Serial.printf("[TEST] [%s:%i] %s\n", file, line, name);
    }
    void reportTestResult(const char *name, bool rtn, const char *file, int line) {
        if(rtn)
            Serial.printf("[TEST:SUCCESS] [%s:%i] %s\n", file, line, name);
        else
            Serial.printf("[TEST:FAIL] [%s:%i] %s\n", file, line, name);
    }

    void reportIgnoredAssert(const char *a, const char *b, bool passed, const char *file, int line) {
        const char *prev = passed ? "passed" : "faild";
        Serial.printf("[IGNORE:ASSERT] [%s:%i] (%s == %s) due to previous assersions %s.\n", file, line, a, b, prev);
    }

    void info(const char *text) {
        Serial.printf("[INFO] %s\n", text);
    }
    void info(const char singleChar) {
        Serial.printf("[INFO] %c\n", singleChar);
    }

    void midiDiff(uint8_t type, uint8_t channel, uint8_t a, uint8_t b, uint16_t count, bool expectation) {
        const char *typeT = "unknown";
        if(type == 0) typeT = "toggle";
        if(type == 1) typeT = "off";
        if(type == 2) typeT = "programm";
        if(type == 3) typeT = "controll";
        Serial.printf("[MIDI] %s%d: %s, channel: %d, a: %d, b: %d\n", expectation ? "-" : "+", count, typeT, channel, a, b);
    }
};

#endif /* ARDUINO_TEST_OUT_ADAPTER_HPP_ */
