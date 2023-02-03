#ifndef ARDUINO_TEST_OUT_ADAPTER_HPP_
#define ARDUINO_TEST_OUT_ADAPTER_HPP_

#include <Arduino.h>
#include "core/test/stdTestOut.hpp"

struct silentAdruinoTestOut : projektionMidi::test::silentStdTestOut {
    void osYield() {
        yield();
    }
};


struct adruinoTestOut : projektionMidi::test::stdTestOut {
    void osYield() {
        yield();
    }
};

#endif /* ARDUINO_TEST_OUT_ADAPTER_HPP_ */
