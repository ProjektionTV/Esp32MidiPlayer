#include "projektionMidi.hpp"

#include "parsers.hpp"
#include "stringTextWalker.hpp"
#include "textWalkerUtil.hpp"
#include "wdhTextWalker.hpp"

void projektionMidi::playStack::push(playStackFrame *frame, errorReciever errorReciever) {
    if(stackIndex >= PROJEKTION_MIDI_MAX_MIDI_STACK_SIZE) {
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

void projektionMidi::projektionMidi::enqueue(std::string text, uint16_t time) {
    queue.push_back({ .text = std::move(text), .time_seconds = time });
}

void projektionMidi::projektionMidi::tick(uint64_t us) {
    if(!playing && !queue.empty()) {
        playNext(us);
    }
    if(playing) {
        bool hasplaying = false;
        for(uint16_t i = 0; i < player.size(); i++) {
            while(player[i].playing && (player[i].haltedTill <= us)) {
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
                        midiHandler::channelMapEntry channel = getMidiChannel(player[i].current->midiChannel);
                        if(channel.handler != nullptr)
                            channel.handler->allOff(channel.channel);
                    }
                    player[i].current->walker->skip();
                    break;
                case '\0':
                /*case ':':*/ // TODO: readd when addig segments
                    if(player[i].pauseOnSpace) {
                        player[i].haltedTill = us + (fourBeatTime / 4);
                        player[i].pauseOnSpace = false;
                        break;
                    }
                    if(player[i].current->mode == PROJEKTION_MIDI_MODE_1) {
                        midiHandler::channelMapEntry channel = getMidiChannel(player[i].current->midiChannel);
                        if(channel.handler != nullptr)
                            channel.handler->allOff(channel.channel);
                    }
                    player[i].jumpToCommand = false;
                    player[i].pop(errorReciever);
                    if(player[i].current != nullptr) player[i].current->walker->skip();
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
        return &tracks[0];
    }
    return &tracks[track];
}

void projektionMidi::projektionMidi::playTrack(uint16_t track) {
    playTrack(getTrack(track));
}

void projektionMidi::projektionMidi::playTrack(playTrackInfo *trackInfo) {
    for(uint16_t i = 0; i < player.size(); i++) {
        if(!player[i].playing) {
            playTrack(trackInfo, &player[i]);
            return;
        }
    }
    playTrack(trackInfo, &player.emplace_back());
}

void projektionMidi::projektionMidi::playTrack(uint16_t track, playStack *stack) {
    playTrack(getTrack(track), stack);
}

void projektionMidi::projektionMidi::playTrack(playTrackInfo *trackInfo, playStack *stack) {
    stack->haltedTill = 0;

    // init frame: bpm & mode
    playStackFrame *psf = new playStackFrame();
    psf->mode = trackInfo->otherMode ? otherMode(baseMode) : baseMode;

    // set Walker for frame
    std::destroy_at(&psf->walkerBackend);
    (*(new (&psf->walkerBackend) std::unique_ptr(std::make_unique<stringTextWalker>(&text))))->setAddress(trackInfo->walkerAddress);

    std::destroy_at(&psf->walker);
    new (&psf->walker) std::unique_ptr(std::make_unique<wdhTextWalker>(psf->walkerBackend.get()));

    // read instrument
    readInstrument(psf->walker.get(), getMidiChannel(psf->midiChannel));
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
        channel.handler->controlChange(channel.channel, 0, 0);
        channel.handler->controlChange(channel.channel, 32, 0);
        channel.handler->programChange(channel.channel, 0);
        channel.handler->controlChange(channel.channel, 7, 127);
        channel.handler->controlChange(channel.channel, 72, 63);
        channel.handler->controlChange(channel.channel, 73, 63);
    }
    if(musicStatusReciever) musicStatusReciever(true);
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
    baseMode = PROJEKTION_MIDI_MODE_1;
    if(walker->peek() == '-') {
        walker->skip();
        baseMode = PROJEKTION_MIDI_MODE_2;
    }

    textWalkerUtil::skipAfterSpaces(walker);
    uint32_t bpm = 0;
    if(textWalkerUtil::startsWith(walker, "bpm")) {
        walker->skip(3);
        bpm = textWalkerUtil::readUInt32(walker);
    }
    if(bpm == 0) {
        bpm = PROJEKTION_MIDI_DEFAULT_BPM;
    }

    textWalkerUtil::skipAfterSpaces(walker);

    fourBeatTime = (uint32_t) (240'000'000.0f / ((float) bpm));
    textWalkerUtil::skipAfterSpaces(walker);

    if(musicStatusReciever) musicStatusReciever(false);
    playing = true;

    // TODO: handle segments
    playTrackInfo pti;
    pti.otherMode = false;
    pti.playOnStart = true;
    pti.walkerAddress = walker->getAddress();
    tracks.emplace_back(pti);
    playTrack(&pti);
    delete walker;
}

void projektionMidi::projektionMidi::addMidiChannel(uint32_t textChannel, midiHandler::eventHandler *handler, uint8_t eventChannel) {
    if(midiChannel.size() <= textChannel) midiChannel.resize(textChannel + 1);
    midiChannel[textChannel] = { .handler = handler, .channel = eventChannel };
    if(handler != nullptr) {
        handler->allOff(eventChannel);
        handler->controlChange(eventChannel, 0, 0);
        handler->controlChange(eventChannel, 32, 0);
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

projektionMidi::playStackFrame::~playStackFrame() {}

projektionMidi::playStack::~playStack() {
    while(stackIndex) popUnsafe();
}

projektionMidi::playTrackInfo::~playTrackInfo() {
    // walkerAddress // TODO: use string textwalker impl
}
