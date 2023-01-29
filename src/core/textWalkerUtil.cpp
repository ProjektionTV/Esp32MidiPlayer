#include "textWalkerUtil.hpp"

uint32_t projektionMidi::textWalkerUtil::readUInt32(textWalker *walker) {
    uint32_t temp = 0;
    char c = walker->peek();
    while(c >= '0' && c <= '9') {
        temp *= 10;
        temp += c - '0';
        walker->skip();
        c = walker->peek();
    }
    return temp;
}

void projektionMidi::textWalkerUtil::skipAfterSpaces(textWalker *walker) {
    char c = walker->peek();
    while(c == ' ') {
        walker->skip();
        c = walker->peek();
    }
}

bool projektionMidi::textWalkerUtil::startsWith(addressableTextWalker *walker, const char *text) {
    auto start = walker->getAddress();
    bool out = startsWithImpl(walker, text);
    walker->setAddress(start);
    walker->deleteAddress(start);
    return out;
}

bool projektionMidi::textWalkerUtil::startsWithImpl(textWalker *walker, const char *text) {
    std::size_t i = 0;
    while(text[i] != '\0')
        if(walker->peek() == text[i++]) walker->skip();
        else return false;
    return true;
}
