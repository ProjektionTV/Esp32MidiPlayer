#include "parsers.hpp"

#include "textWalkerUtil.hpp"

namespace projektionMidi {
    namespace parserUtil {
        uint8_t readOktaveOffset(textWalker *walker) {
            uint8_t offset = 0;
            for(uint8_t i = 0; i < 3; i++) {
                if(walker->peek() == '\'') {
                    offset++;
                    walker->skip();
                } else return offset;
            }
            return offset;
        }
        void readHalbton(textWalker *walker, bool &halbtonC, bool &halbtonB) {
            halbtonC = walker->peek() == '#';
            if(halbtonC) walker->skip();
            halbtonB = walker->peek() == 'b';
            if(halbtonB) walker->skip();
        }
        uint8_t getNoteID(char note, bool &allowHabtonC, bool &allowHabtonB, bool &noteDown) {
            uint8_t noteID = 128;
            switch(note) {
            case 'C': noteDown = true;  allowHabtonB = false;   allowHabtonC = true;    noteID = 48; break;
            case 'D': noteDown = true;  allowHabtonB = true;    allowHabtonC = true;    noteID = 50; break;
            case 'E': noteDown = true;  allowHabtonB = true;    allowHabtonC = false;   noteID = 52; break;
            case 'F': noteDown = true;  allowHabtonB = false;   allowHabtonC = true;    noteID = 53; break;
            case 'G': noteDown = true;  allowHabtonB = true;    allowHabtonC = true;    noteID = 55; break;
            case 'A': noteDown = true;  allowHabtonB = true;    allowHabtonC = true;    noteID = 57; break;
            case 'H': noteDown = true;  allowHabtonB = true;    allowHabtonC = false;   noteID = 59; break;

            case 'c': noteDown = false; allowHabtonB = false;   allowHabtonC = true;    noteID = 60; break;
            case 'd': noteDown = false; allowHabtonB = true;    allowHabtonC = true;    noteID = 62; break;
            case 'e': noteDown = false; allowHabtonB = true;    allowHabtonC = false;   noteID = 64; break;
            case 'f': noteDown = false; allowHabtonB = false;   allowHabtonC = true;    noteID = 65; break;
            case 'g': noteDown = false; allowHabtonB = true;    allowHabtonC = true;    noteID = 67; break;
            case 'a': noteDown = false; allowHabtonB = true;    allowHabtonC = true;    noteID = 69; break;
            case 'h': noteDown = false; allowHabtonB = true;    allowHabtonC = false;   noteID = 71; break;
            }
            return noteID;
        }
        uint8_t convertNote(uint8_t noteId, uint8_t oktavenOffset, bool habtonC, bool habtonB, bool allowHabtonC, bool allowHabtonB, bool noteDown){
            return noteId + (oktavenOffset * 12 * (noteDown ? -1 : 1)) + ((allowHabtonC && habtonC) ? 1 : 0) - ((allowHabtonB && habtonB) ? 1 : 0);
        }
    } /* namespace parserUtil */
} /* namespace projektionMidi */

uint8_t projektionMidi::readNote(char noteChar, textWalker *text) {
    uint8_t oktaveOffset = parserUtil::readOktaveOffset(text);
    bool halbtonC = false;
    bool halbtonB = false;
    parserUtil::readHalbton(text, halbtonC, halbtonB);
    bool noteDown = true;
    bool allowHalbtonC = true;
    bool allowHalbtonB = true;
    uint8_t noteID = parserUtil::getNoteID(noteChar, allowHalbtonC, allowHalbtonB, noteDown);
    if(noteID == 128) return 128;
    uint8_t note = parserUtil::convertNote(noteID, oktaveOffset, halbtonC, halbtonB, allowHalbtonC, allowHalbtonB, noteDown);
    return note;
}

float projektionMidi::readDelay(char readChar, textWalker *text) {
    if(readChar < '9' && readChar > '0') {
        float length = (float) textWalkerUtil::readUInt32(text);
        if(length == 0) length = 4;
        if(text->peek() == '.') {
            text->skip();
            length /= 1.5f;
        }
        return length;
    }
    return 0;
}

void projektionMidi::parser1(projektionMidi *midi, playStack *stack, uint64_t us) {
    char c = stack->current->walker->peek();
    float delay = readDelay(c, stack->current->walker.get());
    if(delay != 0) {
        stack->jumpToCommand = true;
        stack->haltedTill = us + midi->fourBeatTime / delay;
        return;
    }
    stack->current->walker->skip();
    uint8_t note = readNote(c, stack->current->walker.get());
    bool play = true;
    bool pause = false;
    if(note == 128) play = false;
    if(c == 'P' || c == 'p') pause = true;
    if(pause) play = false;
    if(play) {
        stack->lastNamedNote = note;
        midiHandler::channelMapEntry channel = midi->getMidiChannel(stack->midiChannel);
        if(channel.handler != nullptr)
            channel.handler->note(channel.channel, note, stack->current->velocity);
    }
    stack->pauseOnSpace = play || pause;
}

void projektionMidi::parser2(projektionMidi *midi, playStack *stack, uint64_t us) {
    char c = stack->current->walker->peek();
    float delay = readDelay(c, stack->current->walker.get());
    if(delay != 0) {
        stack->haltedTill = us + midi->fourBeatTime / delay;
        return;
    }
    stack->current->walker->skip();
    midiHandler::channelMapEntry channel = midi->getMidiChannel(stack->midiChannel);
    bool hasMidi = channel.handler != nullptr;
    char c2;
    switch(c) {
    case 's':
    case 'S':
        midi->stopAllMidi();
        break;
    case 'l':
    case 'L':
        if(hasMidi)
            channel.handler->note(channel.channel, stack->lastNamedNote, stack->current->velocity);
        break;
    case 'q':
    case 'Q': // bpm
        c2 = stack->current->walker->peek();
        if(c2 < '9' && c2 > '0') {
            uint32_t newV = textWalkerUtil::readUInt32(stack->current->walker.get());
            if(newV == 0) newV = midi->getSettings()->defaultBpm;
            midi->fourBeatTime = (uint32_t) (240'000'000.0f / ((float) newV));
            if(midi->getFXHandler() != nullptr) midi->getFXHandler()->setBpm(newV);
        }
        break;
    case 'v':
    case 'V': // velocity
        c2 = stack->current->walker->peek();
        if(c2 < '9' && c2 > '0') {
            uint32_t newV = textWalkerUtil::readUInt32(stack->current->walker.get());
            if(newV >= 0 && newV < 128)
                stack->current->velocity = newV;
        }
        break;
    case 'x':
    case 'X': // controll 0 // bank MSB
        c2 = stack->current->walker->peek();
        if(c2 < '9' && c2 > '0') {
            uint32_t newV = textWalkerUtil::readUInt32(stack->current->walker.get());
            if(newV >= 0 && newV < 128 && hasMidi)
                channel.handler->controlChange(channel.channel, 0, newV);
        }
        break;
    case 'y':
    case 'Y': // controll 32 // bank LSB
        c2 = stack->current->walker->peek();
        if(c2 < '9' && c2 > '0') {
            uint32_t newV = textWalkerUtil::readUInt32(stack->current->walker.get());
            if(newV >= 0 && newV < 128 && hasMidi)
                channel.handler->controlChange(channel.channel, 32, newV);
        }
        break;
    case 'j':
    case 'J': // controll 72
        c2 = stack->current->walker->peek();
        if(c2 < '9' && c2 > '0') {
            uint32_t newV = textWalkerUtil::readUInt32(stack->current->walker.get());
            if(newV >= 0 && newV < 128 && hasMidi)
                channel.handler->controlChange(channel.channel, 72, newV);
        }
        break;
    case 'o':
    case 'O': // controll 73
        c2 = stack->current->walker->peek();
        if(c2 < '9' && c2 > '0') {
            uint32_t newV = textWalkerUtil::readUInt32(stack->current->walker.get());
            if(newV >= 0 && newV < 128 && hasMidi)
                channel.handler->controlChange(channel.channel, 73, newV);
        }
        break;
    case 'i':
    case 'I': // read instrument
        midi->readInstrument(stack->current->walker.get(), channel);
        break;
    case 'k':
    case 'K':
        c2 = stack->current->walker->peek();
        if(c2 < '9' && c2 > '0') {
            uint32_t channel = textWalkerUtil::readUInt32(stack->current->walker.get());
            stack->midiChannel = channel;
        }
        break;
    case 't':
        c2 = stack->current->walker->peek();
        if(c2 < '9' && c2 > '0') {
            uint32_t track = textWalkerUtil::readUInt32(stack->current->walker.get());
            midi->playTrack(track);
        }
        break;
    case 'T':
        c2 = stack->current->walker->peek();
        if(c2 < '9' && c2 > '0') {
            uint32_t track = textWalkerUtil::readUInt32(stack->current->walker.get());
            midi->playTrack(track, stack);
        }
        break;
    default:
        uint8_t note = readNote(c, stack->current->walker.get());
        if(note != 128) {
            stack->lastNamedNote = note;
            midiHandler::channelMapEntry channel = midi->getMidiChannel(stack->midiChannel);
            if(channel.handler != nullptr)
                channel.handler->note(channel.channel, note, stack->current->velocity);
        }
        break;
    }
}
