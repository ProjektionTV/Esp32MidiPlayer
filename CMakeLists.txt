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
        add_custom_command(
            TARGET tests
            COMMENT "Run tests"
            POST_BUILD
            COMMAND tests
    )
    endif()
endif()
