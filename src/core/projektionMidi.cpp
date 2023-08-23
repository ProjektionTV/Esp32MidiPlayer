#include "projektionMidi.hpp"

#include <algorithm>
#include <cstring>

#include "parsers.hpp"
#include "stringTextWalker.hpp"
#include "textWalkerUtil.hpp"
#include "wdhTextWalker.hpp"

void projektionMidi::playStack::push(playStackFrame *frame, errorReciever errorReciever) {
    if(stackIndex >= stackSize) {
        if(errorReciever) errorReciever("Stack overflow");
        return;
    }
    current = frame;
    frames[stackIndex++] = frame;
    playing = true;
}

void projektionMidi::playStack::pop(errorReciever errorReciever) {
    if(stackIndex == 0) {
        if(errorReciever) errorReciever("Stack underflow");
        return;
    }
    popUnsafe();
}

void projektionMidi::playStack::popUnsafe() {
    delete current;
    if (--stackIndex == 0) {
        playing = false;
        current = nullptr;
    } else {
        current = frames[stackIndex - 1];
    }
}

bool projektionMidi::projektionMidi::enqueue(std::string text, uint16_t time) {
    if(settings.maxQueueSize != 0 && queue.size() >= settings.maxQueueSize) return false;
    queue.push_back({ .text = std::move(text), .time_seconds = time });
    return true;
}

uint64_t projektionMidi::projektionMidi::getWaitTarget() {
    if(!playing || (player.size() == 0)) return 0;
    uint64_t rtn = -1;
    bool doRtn = false;
    for(uint16_t i = 0; i < player.size(); i++) {
        if(player[i].playing) {
            rtn = std::min(rtn, player[i].haltedTill);
            doRtn = true;
        }
    }
    return doRtn ? rtn : 0;
}

void projektionMidi::projektionMidi::tick(uint64_t us) {
    if(!playing && !queue.empty()) {
        playNext(us);
    }
    if(playing) {
        if(fxHandler != nullptr) fxHandler->tick(us);
        std::size_t parsedTokens = 0;
        bool hasplaying = false;
        for(uint16_t i = 0; i < player.size(); i++) {
            while(player[i].playing && (player[i].haltedTill <= us)) {
                if(settings.maxTokensToParse != 0 && parsedTokens >= settings.maxTokensToParse) return;
                char c = player[i].current->walker->peek();
                switch(c) {
                case 'M':
                case 'm':
                    player[i].jumpToCommand = false;
                    player[i].current->mode = otherMode(player[i].current->mode);
                    player[i].current->walker->skip();
                    break;
                case ' ':
                    player[i].jumpToCommand = false;
                    if(player[i].pauseOnSpace) {
                        player[i].haltedTill = us + (fourBeatTime / 4);
                        player[i].pauseOnSpace = false;
                        break;
                    }
                    if(player[i].current->mode == PROJEKTION_MIDI_MODE_1) {
                        midiHandler::channelMapEntry channel = getMidiChannel(player[i].midiChannel);
                        if(channel.handler != nullptr)
                            channel.handler->allOff(channel.channel);
                    }
                    player[i].current->walker->skip();
                    break;
                case '\0':
                case ':':
                    if(player[i].pauseOnSpace) {
                        player[i].haltedTill = us + (fourBeatTime / 4);
                        player[i].pauseOnSpace = false;
                        break;
                    }
                    if(player[i].current->mode == PROJEKTION_MIDI_MODE_1) {
                        midiHandler::channelMapEntry channel = getMidiChannel(player[i].midiChannel);
                        if(channel.handler != nullptr)
                            channel.handler->allOff(channel.channel);
                    }
                    player[i].jumpToCommand = false;
                    player[i].pop(errorReciever);
                    break;
                default:
                    if(player[i].jumpToCommand) { player[i].current->walker->skip(); break; }
                    player[i].pauseOnSpace = false;
                    if(player[i].current->mode == PROJEKTION_MIDI_MODE_1) {
                        parser1(this, &player[i], us);
                    } else {
                        parser2(this, &player[i], us);
                    }
                    break;
                }
                parsedTokens++;
            }
            hasplaying |= player[i].playing;
        }
        if(!hasplaying || us >= time_end_us) {
            cleanUpPlay();
        }
    }
}

void projektionMidi::projektionMidi::kill() {
    queue.clear();
    time_end_us = 0;
}


projektionMidi::playTrackInfo *projektionMidi::projektionMidi::getTrack(uint16_t track) {
    if(track < 0 || track >= tracks.size()) {
        if(errorReciever) errorReciever("Invalid Track");
        return nullptr;
    }
    return &tracks[track];
}

void projektionMidi::projektionMidi::playTrack(uint16_t track) {
    playTrack(getTrack(track));
}

void projektionMidi::projektionMidi::playTrack(playTrackInfo *trackInfo) {
    for(uint16_t i = 0; i < player.size(); i++) {
        if(!player[i].playing) {
            player[i].midiChannel = settings.defaultMidiChannel;
            playTrack(trackInfo, &player[i]);
            return;
        }
    }
    if(settings.maxTracks != 0 && player.size() >= settings.maxTracks) {
        if(errorReciever) errorReciever("Too many simultaneous tracks");
        return;
    }
    playStack *stack = &player.emplace_back(settings.midiStackSize);
    stack->midiChannel = settings.defaultMidiChannel;
    playTrack(trackInfo, stack);
}

void projektionMidi::projektionMidi::playTrack(uint16_t track, playStack *stack) {
    playTrack(getTrack(track), stack);
}

void projektionMidi::projektionMidi::playTrack(playTrackInfo *trackInfo, playStack *stack) {
    if(trackInfo == nullptr) return;
    stack->haltedTill = 0;

    // init frame: mode
    playStackFrame *psf = new playStackFrame();
    psf->mode = trackInfo->otherMode ? otherMode(baseMode) : baseMode;

    // set Walker for frame
    std::destroy_at(&psf->walkerBackend);
    (*(new (&psf->walkerBackend) std::unique_ptr(std::make_unique<stringTextWalker>(&text))))->setAddress(trackInfo->walkerAddress);

    std::destroy_at(&psf->walker);
    new (&psf->walker) std::unique_ptr(std::make_unique<wdhTextWalker>(psf->walkerBackend.get()));

    // read instrument
    readInstrument(psf->walker.get(), getMidiChannel(stack->midiChannel));
    textWalkerUtil::skipAfterSpaces(psf->walker.get());

    stack->push(psf, errorReciever);
}


void projektionMidi::projektionMidi::cleanUpPlay() {
    playing = false;
    player.clear();
    tracks.clear();
    for(uint32_t ch = 0; ch < midiChannel.size(); ch++) {
        midiHandler::channelMapEntry channel = getMidiChannel(ch);
        if(channel.handler == nullptr) continue;
        channel.handler->allOff(channel.channel);
        channel.handler->controlChange(channel.channel, 0, settings.midi_gm_bank_msb);
        channel.handler->controlChange(channel.channel, 32, settings.midi_gm_bank_lsb);
        channel.handler->programChange(channel.channel, 0);
        channel.handler->controlChange(channel.channel, 7, 127);
        channel.handler->controlChange(channel.channel, 72, 63);
        channel.handler->controlChange(channel.channel, 73, 63);
    }
    if(queue.size() == 0) {
        if(musicStatusReciever) musicStatusReciever(true);
        if(fxHandler != nullptr) fxHandler->stop();
    }
}

void projektionMidi::projektionMidi::stopAllMidi() {
    for(uint32_t ch = 0; ch < midiChannel.size(); ch++) {
        midiHandler::channelMapEntry channel = getMidiChannel(ch);
        if(channel.handler == nullptr) continue;
        channel.handler->allOff(channel.channel);
    }
}

void projektionMidi::projektionMidi::playNext(uint64_t us) {
    playInfo info = queue.front();
    text = std::move(info.text);
    time_end_us = (info.time_seconds * 1'000'000) + us;
    queue.pop_front();

    stringTextWalker *walker = new stringTextWalker(&text);

    textWalkerUtil::skipAfterSpaces(walker);
    if(walker->peek() == '~') {
        walker->skip();
        char c = walker->peek();
        if(c <= '9' && c >= '0') {
            uint32_t presetID = textWalkerUtil::readUInt32(walker);
            delete walker;
            bool isPresetERR = false;
            if(presetID < presets.size()) {
                midiPreset preset = presets[presetID];
                if(preset.midi[0] != '\0') {
                    text = preset.midi;
                    walker = new stringTextWalker(&text);
                } else isPresetERR = true;
            } else isPresetERR = true;
            if(isPresetERR) {
                if(errorReciever) errorReciever("Unknown Preset");
                return;
            }
        }
    }

    textWalkerUtil::skipAfterSpaces(walker);
    baseMode = settings.defaultMode;
    if(walker->peek() == '-') {
        walker->skip();
        baseMode = otherMode(settings.defaultMode);
    }

    textWalkerUtil::skipAfterSpaces(walker);
    uint32_t bpm = 0;
    if(textWalkerUtil::startsWith(walker, "bpm")) {
        walker->skip(3);
        bpm = textWalkerUtil::readUInt32(walker);
    }
    if(bpm == 0) {
        bpm = settings.defaultBpm;
    }

    textWalkerUtil::skipAfterSpaces(walker);

    fourBeatTime = (uint32_t) (240'000'000.0f / ((float) bpm));
    if(fxHandler != nullptr) fxHandler->setBpm(bpm);
    textWalkerUtil::skipAfterSpaces(walker);

    if(musicStatusReciever) musicStatusReciever(false);
    playing = true;

    uint16_t tracki = 0;
    while(walker->peek() != '\0') {
        // register track
        playTrackInfo &pti = tracks.emplace_back();
        // collect data
        // play on start
        pti.playOnStart = walker->peek() != '!';
        if(!pti.playOnStart) walker->skip();
        // mode
        pti.otherMode = walker->peek() == '-';
        if(pti.otherMode) walker->skip();
        // addres
        pti.walkerAddress = walker->getAddress();
        // start track when it should
        if(pti.playOnStart) playTrack(tracki);
        // skip to next track or end
        char c = walker->peek();
        while(c != '\0' && c != ':') {
            walker->skip();
            c = walker->peek();
        }
        walker->skip();
        tracki++;
    }
    delete walker;
    if(fxHandler != nullptr) fxHandler->start();
}

void projektionMidi::projektionMidi::addMidiChannel(uint32_t textChannel, midiHandler::midiEventHandler *handler, uint8_t eventChannel) {
    if(midiChannel.size() <= textChannel) midiChannel.resize(textChannel + 1);
    midiChannel[textChannel] = { .handler = handler, .channel = eventChannel };
    if(handler != nullptr) {
        handler->allOff(eventChannel);
        handler->controlChange(eventChannel, 0, settings.midi_gm_bank_msb);
        handler->controlChange(eventChannel, 32, settings.midi_gm_bank_lsb);
        handler->programChange(eventChannel, 0);
        handler->controlChange(eventChannel, 7, 127);
        handler->controlChange(eventChannel, 72, 63);
        handler->controlChange(eventChannel, 73, 63);
    }
}

projektionMidi::midiHandler::channelMapEntry projektionMidi::projektionMidi::getMidiChannel(uint32_t channel) {
    return channel > midiChannel.size() ? nullChannel : midiChannel[channel];
}

void projektionMidi::projektionMidi::addPreset(uint32_t id, const char *text) {
    if(presets.size() <= id) presets.resize(id + 1);
    presets[id] = { .midi = text };
}

void projektionMidi::projektionMidi::addInstrument(midiInstrument instrument) {
    instruments.emplace_back(instrument);
}

void projektionMidi::projektionMidi::readInstrument(addressableTextWalker *walker, midiHandler::channelMapEntry channel) {
    char c = walker->peek();
    if(c < '9' && c > '0') {
        uint32_t newV = textWalkerUtil::readUInt32(walker);
        if(newV >= 0 && newV < 128 && channel.handler != nullptr)
            channel.handler->programChange(channel.channel, newV);
        return;
    }
    auto start = walker->getAddress();
    for(std::size_t i = 0; i < instruments.size(); i++) {
        midiInstrument ins = instruments[i];
        if(textWalkerUtil::startsWithImpl(walker, ins.name)) {
            walker->deleteAddress(start);
            if(channel.handler != nullptr) {
                channel.handler->controlChange(channel.channel, 0, ins.bankMsb);
                channel.handler->controlChange(channel.channel, 32, ins.bankLsb);
                channel.handler->programChange(channel.channel, ins.program);
            }
            return;
        }
        walker->setAddress(start);
    }
    walker->deleteAddress(start);
}

void projektionMidi::projektionMidi::setErrorReciever(::projektionMidi::errorReciever errorReciever_) {
    errorReciever = errorReciever_;
}
void projektionMidi::projektionMidi::setMusicStatusReciever(::projektionMidi::musicStatusReciever musicStatusReciever_) {
    musicStatusReciever = musicStatusReciever_;
}

projektionMidi::playStack::~playStack() {
    while(stackIndex) popUnsafe();
    free(frames);
}

projektionMidi::playStack::playStack(uint8_t stackSize) :
        stackSize(stackSize) {
    frames = (playStackFrame **) malloc(sizeof(playStackFrame *) * stackSize);
}

projektionMidi::playStack::playStack(playStack &&other) {
    playing = other.playing;
    haltedTill = other.haltedTill;
    pauseOnSpace = other.pauseOnSpace;
    jumpToCommand = other.jumpToCommand;
    lastNamedNote = other.lastNamedNote;
    midiChannel = other.midiChannel;
    stackIndex = other.stackIndex;
    std::swap(stackSize, other.stackSize);
    std::swap(frames, other.frames);
    current = other.current;
    other.stackIndex = 0;
}

projektionMidi::playStack &projektionMidi::playStack::operator=(playStack &&other) {
    playing = other.playing;
    haltedTill = other.haltedTill;
    pauseOnSpace = other.pauseOnSpace;
    jumpToCommand = other.jumpToCommand;
    lastNamedNote = other.lastNamedNote;
    midiChannel = other.midiChannel;
    stackIndex = other.stackIndex;
    std::swap(stackSize, other.stackSize);
    std::swap(frames, other.frames);
    std::swap(current, other.current);
    std::swap(stackIndex, other.stackIndex);
    return *this;
}

projektionMidi::playTrackInfo::~playTrackInfo() {
    // walkerAddress // TODO: use string textwalker impl & impl constructors
}

projektionMidi::playTrackInfo::playTrackInfo(playTrackInfo &&other) {
    playOnStart = other.playOnStart;
    otherMode = other.otherMode;
    walkerAddress = other.walkerAddress;
    other.walkerAddress = textWalkerAddressHandle();
}

uint8_t projektionMidi::projektionMidi::bufferOperation(const char *directText, const char *name, uint16_t playLength, uint32_t maxSize, uint32_t &currSize) {
    bufferData *buffer = nullptr;
    // collect buffer of user or nullptr
    for(auto &data : buffers)
        if(data.name == name) {
            buffer = &data;
            break;
        }
    uint8_t out = 0;
    std::size_t textLen = std::strlen(directText);
    std::size_t i = 0;
    if(i < textLen && directText[i] == ';') { // buffer operation
        i++;
        bool append = buffer != nullptr;
        if(i < textLen && directText[i] == 'l') { // buffer delete operation
            i++;
            if(buffer != nullptr) { // delete buffer
                buffers.erase(std::remove_if(buffers.begin(), buffers.end(), [name](const auto &e){ return e.name == name; }), buffers.end());
                buffer = nullptr;
                //
                append = false;
                out |= (1 & 3) << 0;
                currSize = 0;
            } else { // no delete
                out |= (2 & 3) << 0;
            }
        }
        if(i < textLen && directText[i] == 'n') { // buffer create operation
            i++;
            if(buffer == nullptr) { // create buffer
                if(settings.maxBuffers != 0 && buffers.size() >= settings.maxBuffers) {
                    out |= (3 & 3) << 2;
                    out |= 1 << 5; // refund
                    return out;
                } else {
                    buffer = &buffers.emplace_back();
                    buffer->name = name;
                    append = true;
                    out |= (1 & 3) << 2;
                }
                currSize = 0;
            } else { // no create
                out |= (2 & 3) << 2;
            }
        }
        if(append) { // append to buffer
            buffer->data.append(directText + i, std::min(textLen - i, maxSize - buffer->data.size()));
            if(buffer->data.size() < maxSize) buffer->data += ' ';
            out |= 1 << 4; // modify
            out |= 1 << 5; // refund
            currSize = buffer->data.size();
        } else { // play
            if(!enqueue(directText + i, playLength)) {
                out |= 1 << 5; // refund
                out |= 1 << 6; // failed enqueued
            }
        }
    } else if(buffer != nullptr) { // play "buffer + this"
        buffer->data += directText + i;
        if(!enqueue(buffer->data, playLength)) {
            out |= 1 << 5; // refund
            out |= 1 << 6; // failed enqueued
        }
        // delete buffer
        buffers.erase(std::remove_if(buffers.begin(), buffers.end(), [name](const auto &e){ return e.name == name; }), buffers.end());
        buffer = nullptr;
    } else { // play "this"
        if(!enqueue(directText + i, playLength)) {
            out |= 1 << 5; // refund
            out |= 1 << 6; // failed enqueued
        }
    }
    return out;
}

projektionMidi::projektionMidiSettings *projektionMidi::projektionMidi::getSettings() {
    return &settings;
}

void projektionMidi::projektionMidi::setFXHandler(FXHandler *fxHandler_) {
    fxHandler = fxHandler_;
}

projektionMidi::FXHandler *projektionMidi::projektionMidi::getFXHandler() {
    return fxHandler;
}

void projektionMidi::projektionMidi::addPresetsFromOther(projektionMidi *other) {
    std::size_t ts = presets.size();
    std::size_t os = other->presets.size();
    presets.resize(ts + os);
    for(std::size_t ti = ts, oi = 0; oi < os; oi++, ti++) presets[ti] = other->presets[oi];
}

void projektionMidi::projektionMidi::addInstrumentsFromOther(projektionMidi *other) {
    std::size_t ts = instruments.size();
    std::size_t os = other->instruments.size();
    instruments.resize(ts + os);
    for(std::size_t ti = ts, oi = 0; oi < os; oi++, ti++) instruments[ti] = other->instruments[oi];
}

void projektionMidi::projektionMidi::addMidiChannels(uint32_t textChannel, midiHandler::midiEventHandler *handler, uint8_t eventChannel, uint8_t count) {
    if(midiChannel.size() < textChannel + count) midiChannel.resize(textChannel + count);
    for(uint8_t i = 0; i < count; i++) midiChannel[textChannel + i] = { .handler = handler, .channel = (uint8_t) (eventChannel + i) };
    if(handler != nullptr) {
        for(uint8_t i = 0; i < count; i++) {
            handler->allOff(eventChannel + i);
            handler->controlChange(eventChannel + i, 0, settings.midi_gm_bank_msb);
            handler->controlChange(eventChannel + i, 32, settings.midi_gm_bank_lsb);
            handler->programChange(eventChannel + i, 0);
            handler->controlChange(eventChannel + i, 7, 127);
            handler->controlChange(eventChannel + i, 72, 63);
            handler->controlChange(eventChannel + i, 73, 63);
        }
    }
}
