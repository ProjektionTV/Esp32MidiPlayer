cmake_minimum_required(VERSION 3.20)

project(ProjektionMidi)

option(projektionMidiBuildTests "builds the tests from projektionMidi" ON)
option(projektionMidiRunTests "run the tests from projektionMidi only when build" ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ProjektionMidiCoreSource
    src/core/projektionMidi.cpp
    src/core/parsers.cpp
    src/core/stringTextWalker.cpp
    src/core/wdhTextWalker.cpp
    src/core/textWalkerUtil.cpp
)

add_library(ProjektionMidi STATIC ${ProjektionMidiCoreSource})


if(projektionMidiBuildTests)
    add_executable(tests defaultTests.cpp)
    target_link_libraries(tests PUBLIC ProjektionMidi)
    if(projektionMidiRunTests)
        set(TEST_FLAG_FILE tests.PASSED)
        add_custom_command(
            TARGET tests
            COMMENT "delete flag"
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove -f ${TEST_FLAG_FILE}
        )
        add_custom_command(
            OUTPUT ${TEST_FLAG_FILE}
            COMMENT "run tests"
            DEPENDS tests
            COMMAND tests
            COMMAND ${CMAKE_COMMAND} -E touch ${TEST_FLAG_FILE}
        )
        add_custom_target(run_tests ALL DEPENDS ${TEST_FLAG_FILE})
    endif()
endif()
