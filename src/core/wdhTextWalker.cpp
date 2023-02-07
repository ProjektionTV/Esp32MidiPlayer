#include "wdhTextWalker.hpp"

#include <utility>
#include <cstdlib>
#include "textWalkerUtil.hpp"

char projektionMidi::wdhTextWalker::peek() {
    char curr = '\0';
    while(true) {
        curr = text->peek();
        if(state.wdh_rdy) {
            switch(curr) {
            case 'w':
            case 'W':
                if(state.wdh_rtndo) {
                    text->setAddress(state.wdh_do ? state.wdh_start : state.wdh_rtn);
                    if(state.wdh_do) state.wdh_do--;
                    state.wdh_rtndo = state.wdh_do ? true : false;
                    if(!state.wdh_rtndo && state.wdh_rtn_out) state.wdh_rdy = false;
                } else {
                    text->skip();
                    text->deleteAddress(state.wdh_rtn);
                    state.wdh_rtn = text->getAddress();
                    text->setAddress(state.wdh_start);
                    state.wdh_rtndo = true;
                    state.wdh_do = 0;
                    state.wdh_rtn_out = true;
                }
                break;
            case 'n':
            case 'N':
                text->skip();
                if(state.wdh_rtndo) text->setAddress(state.wdh_do ? state.wdh_start : state.wdh_rtn);
                if(state.wdh_do) state.wdh_do--;
                state.wdh_rtndo = state.wdh_do ? true : false;
                if(!state.wdh_rtndo && state.wdh_rtn_out) state.wdh_rdy = false;
                break;
            case 'u':
            case 'U':
                if(state.wdh_rtndo) {
                    text->setAddress(state.wdh_do ? state.wdh_start : state.wdh_rtn);
                    if(state.wdh_do) state.wdh_do--;
                    state.wdh_rtndo = state.wdh_do ? true : false;
                    if(!state.wdh_rtndo && state.wdh_rtn_out) state.wdh_rdy = false;
                } else {
                    state.wdh_rtndo = true;
                    state.wdh_do = 1;
                    text->skip();
                    if(text->peek() == '-') {
                        text->skip();
                        state.wdh_do = textWalkerUtil::readUInt32(text);
                        if(state.wdh_do == 0) state.wdh_do = 1;
                    }
                    text->deleteAddress(state.wdh_rtn);
                    state.wdh_rtn = text->getAddress();
                    text->setAddress(state.wdh_start);
                    state.wdh_do--;
                }
                break;
            default:
                return curr;
                break;
            }
        } else {
            if(curr == 'w' || curr == 'W') {
                text->skip();
                text->deleteAddress(state.wdh_rtn);
                text->deleteAddress(state.wdh_start);
                state.wdh_start = text->getAddress();
                state.wdh_rtn = text->getAddress();
                state.wdh_do = 0;
                state.wdh_rdy = true;
                state.wdh_rtndo = false;
                state.wdh_rtn_out = false;
            } else return curr;
        }
    }
}

char projektionMidi::wdhTextWalker::next() {
    char o = peek();
    text->skip();
    return o;
}

projektionMidi::wdhTextWalker::wdhTextWalker(addressableTextWalker *text):
        text(text) {
    state.wdh_do = 0;
    state.wdh_rdy = false;
    state.wdh_rtn = text->getAddress();
    state.wdh_rtndo = 0;
    state.wdh_start = text->getAddress();
    state.wdh_rtn_out = false;
}

projektionMidi::wdhTextWalker::~wdhTextWalker() {
    text->deleteAddress(state.wdh_rtn);
    text->deleteAddress(state.wdh_start);
}

projektionMidi::wdhTextWalker::wdhTextWalker(wdhTextWalker &&other):
        text(other.text),
        state(other.state) {
    other.state.wdh_rtn = textWalkerAddressHandle();
    other.state.wdh_start = textWalkerAddressHandle();
}

projektionMidi::wdhTextWalker &projektionMidi::wdhTextWalker::operator=(projektionMidi::wdhTextWalker &&other) {
    text = other.text;
    std::swap(state, other.state);
    return *this;
}

projektionMidi::textWalkerAddressHandle projektionMidi::wdhTextWalker::getAddress() {
    wdhTextWalkerAddress *result = (wdhTextWalkerAddress *) malloc(sizeof(wdhTextWalkerAddress));
    result->i = text->getAddress();
    result->state = state;

    result->state.wdh_rtn = text->copyAddress(state.wdh_rtn);
    result->state.wdh_start = text->copyAddress(state.wdh_start);
    return textWalkerAddressHandle{result};
}

void projektionMidi::wdhTextWalker::setAddress(textWalkerAddressHandle address) {
    if(address.p != nullptr) {
        wdhTextWalkerAddress *other = (wdhTextWalkerAddress *) address.p;
        text->setAddress(other->i);
        state = other->state;

        state.wdh_rtn = text->copyAddress(other->state.wdh_rtn);
        state.wdh_start = text->copyAddress(other->state.wdh_start);
    }
}

void projektionMidi::wdhTextWalker::deleteAddress(textWalkerAddressHandle address) {
    if(address.p != nullptr) {
        wdhTextWalkerAddress *curr = (wdhTextWalkerAddress *) address.p;
        text->deleteAddress(curr->i);
        text->deleteAddress(curr->state.wdh_rtn);
        text->deleteAddress(curr->state.wdh_start);
        free(address.p);
    }
}

