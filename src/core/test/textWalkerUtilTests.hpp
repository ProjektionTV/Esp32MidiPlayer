#ifndef PROJEKTION_MIDI_TEST_TEXT_WALKER_UTIL_TESTS_HPP_
#define PROJEKTION_MIDI_TEST_TEXT_WALKER_UTIL_TESTS_HPP_

#include "../stringTextWalker.hpp"
#include "testUtil.hpp"
#include "textWalkerTestUtil.hpp"
#include "../textWalkerUtil.hpp"

// readUInt32 tests
TEST_START(textWalkerUtil_readUint_00)
    std::string *text = new std::string("1");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::textWalkerUtil::readUInt32(stringWalker), 1, "Read data from text walker with jump didn't match the number '1'")
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_readUint_01_multiChar)
    std::string *text = new std::string("10");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::textWalkerUtil::readUInt32(stringWalker), 10, "Read data from text walker with jump didn't match the number '10'")
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_readUint_02_textAfter)
    std::string *text = new std::string("10a");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::textWalkerUtil::readUInt32(stringWalker), 10, "Read data from text walker with jump didn't match the number '10'")
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_readUint_03_textAfterAndNum)
    std::string *text = new std::string("10a5");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::textWalkerUtil::readUInt32(stringWalker), 10, "Read data from text walker with jump didn't match the number '10'")
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_readUint_04_textBefore)
    std::string *text = new std::string(" 10");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_CUSTOM_MESSAGE(projektionMidi::textWalkerUtil::readUInt32(stringWalker), 0, "Read data from text walker with jump didn't match the number '0'")
    delete stringWalker;
    delete text;
TEST_END

// skipAfterSpaces tests
TEST_START(textWalkerUtil_skipAfterSpace_00_noSpace)
    std::string *text = new std::string("abc");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    projektionMidi::textWalkerUtil::skipAfterSpaces(stringWalker);
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "abc"))
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_skipAfterSpace_01_singleSpace)
    std::string *text = new std::string(" abc");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    projektionMidi::textWalkerUtil::skipAfterSpaces(stringWalker);
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "abc"))
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_skipAfterSpace_02_multipleSpace)
    std::string *text = new std::string("   abc");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    projektionMidi::textWalkerUtil::skipAfterSpaces(stringWalker);
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "abc"))
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_skipAfterSpace_03_spaceWithAfterSpace)
    std::string *text = new std::string(" a b c ");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    projektionMidi::textWalkerUtil::skipAfterSpaces(stringWalker);
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "a b c "))
    delete stringWalker;
    delete text;
TEST_END

// startsWithImpl tests
TEST_START(textWalkerUtil_startsWithImpl_00_singleChar)
    std::string *text = new std::string("ab");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_TRUE(projektionMidi::textWalkerUtil::startsWithImpl(stringWalker, "a"))
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "b"))
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_startsWithImpl_01_multipleChar)
    std::string *text = new std::string("abcxyz");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_TRUE(projektionMidi::textWalkerUtil::startsWithImpl(stringWalker, "abc"))
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "xyz"))
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_startsWithImpl_02_noChar)
    std::string *text = new std::string("bc");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_FALSE(projektionMidi::textWalkerUtil::startsWithImpl(stringWalker, "abc"))
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "bc"))
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_startsWithImpl_03_oneCharCorrect)
    std::string *text = new std::string("acd");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_FALSE(projektionMidi::textWalkerUtil::startsWithImpl(stringWalker, "abc"))
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "cd"))
    delete stringWalker;
    delete text;
TEST_END

// startsWith tests
TEST_START(textWalkerUtil_startsWith_00_singleChar)
    std::string *text = new std::string("ab");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_TRUE(projektionMidi::textWalkerUtil::startsWith(stringWalker, "a"))
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "ab"))
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_startsWith_01_multipleChar)
    std::string *text = new std::string("abcxyz");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_TRUE(projektionMidi::textWalkerUtil::startsWith(stringWalker, "abc"))
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "abcxyz"))
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_startsWith_02_noChar)
    std::string *text = new std::string("bc");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_FALSE(projektionMidi::textWalkerUtil::startsWithImpl(stringWalker, "abc"))
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "bc"))
    delete stringWalker;
    delete text;
TEST_END

TEST_START(textWalkerUtil_startsWith_03_oneCharCorrect)
    std::string *text = new std::string("acd");
    projektionMidi::addressableTextWalker *stringWalker = new projektionMidi::stringTextWalker(text);
    ASSERT_FALSE(projektionMidi::textWalkerUtil::startsWithImpl(stringWalker, "abc"))
    ASSERT_TRUE(projektionMidi::test::testTextWalker::expect(stringWalker, "cd"))
    delete stringWalker;
    delete text;
TEST_END

TESTS_SUB(textWalkerUtilTests)
    // test readUInt32
    TEST(testOut, textWalkerUtil_readUint_00)
    TEST(testOut, textWalkerUtil_readUint_01_multiChar)
    TEST(testOut, textWalkerUtil_readUint_02_textAfter)
    TEST(testOut, textWalkerUtil_readUint_03_textAfterAndNum)
    TEST(testOut, textWalkerUtil_readUint_04_textBefore)

    // test skipAfterSpaces
    TEST(testOut, textWalkerUtil_skipAfterSpace_00_noSpace)
    TEST(testOut, textWalkerUtil_skipAfterSpace_01_singleSpace)
    TEST(testOut, textWalkerUtil_skipAfterSpace_02_multipleSpace)
    TEST(testOut, textWalkerUtil_skipAfterSpace_03_spaceWithAfterSpace)

    // test startsWithImpl
    TEST(testOut, textWalkerUtil_startsWithImpl_00_singleChar)
    TEST(testOut, textWalkerUtil_startsWithImpl_01_multipleChar)
    TEST(testOut, textWalkerUtil_startsWithImpl_02_noChar)
    TEST(testOut, textWalkerUtil_startsWithImpl_03_oneCharCorrect)

    // test startsWith
    TEST(testOut, textWalkerUtil_startsWith_00_singleChar)
    TEST(testOut, textWalkerUtil_startsWith_01_multipleChar)
    TEST(testOut, textWalkerUtil_startsWith_02_noChar)
    TEST(testOut, textWalkerUtil_startsWith_03_oneCharCorrect)
TEST_SUB_END

#endif /* PROJEKTION_MIDI_TEST_TEXT_WALKER_UTIL_TESTS_HPP_ */
