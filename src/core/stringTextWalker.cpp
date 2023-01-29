#include "stringTextWalker.hpp"

bool projektionMidi::stringTextWalker::notAtEnd() {
    return i < text->length();
}

void projektionMidi::stringTextWalker::setAddress(textWalkerAddressHandle address) {
    i = address.n;
}

void projektionMidi::stringTextWalker::deleteAddress(textWalkerAddressHandle address) {}

projektionMidi::textWalkerAddressHandle projektionMidi::stringTextWalker::getAddress() {
    textWalkerAddressHandle result;
    result.n = i;
    return result;
}

char projektionMidi::stringTextWalker::next() {
    return notAtEnd() ? text->at(i++) : '\0';
}

char projektionMidi::stringTextWalker::peek() {
    return notAtEnd() ? text->at(i) : '\0';
}

void projektionMidi::stringTextWalker::skip(uint16_t a) {
    i += a;
}

projektionMidi::stringTextWalker::stringTextWalker(std::string *text):
        text(text),
        i(0) {
}
projektionMidi::stringTextWalker::~stringTextWalker() {
}
