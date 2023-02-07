#ifndef PROJEKTION_MIDI_PROJEKTION_MIDI_HPP_
#define PROJEKTION_MIDI_PROJEKTION_MIDI_HPP_

#include <cinttypes>
#include <deque>
#include <memory>
#include <string>
#include <vector>

#include "midiHandler.hpp"
#include "textWalker.hpp"

#define PROJEKTION_MIDI_MAX_MIDI_STACK_SIZE 3

#define PROJEKTION_MIDI_MODE_1 false
#define PROJEKTION_MIDI_MODE_2 true

namespace projektionMidi {

    typedef void (*errorReciever)(const char *text);
    typedef void (*musicStatusReciever)(bool musicStatus);

    struct playInfo {
        std::string text;
        uint16_t time_seconds;
    };

    struct playStackFrame {
        std::unique_ptr<addressableTextWalker> walkerBackend;
        std::unique_ptr<addressableTextWalker> walker;

        // stack player info
        bool mode = PROJEKTION_MIDI_MODE_1;
        uint8_t velocity = 127;
    };

    struct playStack {
        private:
        uint8_t stackIndex = 0;
        void popUnsafe();
        public:
        playStack() = default;
        playStack(const playStack &) = delete;
        playStack(playStack &&);
        playStack &operator=(const playStack &) = delete;
        playStack &operator=(playStack &&);
        playStackFrame *frames[PROJEKTION_MIDI_MAX_MIDI_STACK_SIZE];
        playStackFrame *current = nullptr;
        void push(playStackFrame *frame, ::projektionMidi::errorReciever errorReciever);
        void pop(::projektionMidi::errorReciever errorReciever);
        ~playStack();

        // general player info
        bool playing = false;
        uint64_t haltedTill = 0;
        bool pauseOnSpace = false;
        bool jumpToCommand = false;
        uint8_t lastNamedNote = 0;
        uint16_t midiChannel = 1;
    };

    struct playTrackInfo {
        ::projektionMidi::textWalkerAddressHandle walkerAddress;
        bool playOnStart;
        bool otherMode;
        playTrackInfo() = default;
        playTrackInfo(playTrackInfo &&);
        playTrackInfo(const playTrackInfo &) = delete;
        playTrackInfo &operator=(const playTrackInfo &) = delete;
        playTrackInfo &operator=(playTrackInfo &&) = delete;
        ~playTrackInfo();
    };

    struct midiPreset {
        const char* midi;
    };

    struct midiInstrument {
        const char* name;
        uint8_t program;
        uint8_t bankLsb;
        uint8_t bankMsb;
    };

    struct bufferData {
        std::string data;
        std::string name;
    };

    struct projektionMidiSettings {
        uint16_t defaultBpm = 240;
        bool defaultMode = PROJEKTION_MIDI_MODE_1;
        std::size_t maxQueueSize = 0; // 0: unlimeted
        std::size_t maxBuffers = 0; // 0: unlimeted
        uint16_t maxTracks = 0; // 0: unlimeted
        uint16_t defaultMidiChannel = 1;
    };

    class projektionMidi {
        private:
        std::deque<playInfo> queue;
        std::vector<bufferData> buffers;

        std::vector<midiInstrument> instruments;

        std::vector<midiHandler::channelMapEntry> midiChannel;
        std::vector<midiPreset> presets;

        std::vector<playStack> player;
        std::vector<playTrackInfo> tracks;
        std::string text;
        uint64_t time_end_us;
        bool baseMode = PROJEKTION_MIDI_MODE_1;

        bool playing = false;

        projektionMidiSettings settings;

        void playNext(uint64_t us);
        void cleanUpPlay();

        constexpr static midiHandler::channelMapEntry nullChannel = { .handler = nullptr, .channel = 0 };
        ::projektionMidi::errorReciever errorReciever = nullptr;
        ::projektionMidi::musicStatusReciever musicStatusReciever = nullptr;

        public:

        void addPreset(uint32_t id, const char *text);
        void addInstrument(midiInstrument instrument);
        void setErrorReciever(::projektionMidi::errorReciever errorReciever_);
        void setMusicStatusReciever(::projektionMidi::musicStatusReciever musicStatusReciever_);

        void addMidiChannel(uint32_t textChannel, midiHandler::eventHandler *handler, uint8_t eventChannel);
        projektionMidiSettings *getSettings();

        /**
         * @returns if the enqueueing was successful
         */
        bool enqueue(std::string text, uint16_t time);
        void kill();
        void tick(uint64_t us);

        /**
         * 0: buffer deleted 0,
         * 1: buffer deleted 1,
         * 2: buffer created 0,
         * 3: buffer created 1,
         * 4: buffer modified,
         * 5: refund
         * 6: failed enqueue
         *
         * buffer deleted:
         * 0: no delete,
         * 1: success,
         * 2: no buffer
         *
         * buffer created:
         * 0: no create,
         * 1: success,
         * 2: has alredy an buffer (append),
         * 3: no bufferslots free (canceled)
         * 
         * @param directText will be copied
         * @param name will be copied
         * @param playLength time to play
         * @param maxSize maximum buffer size
         * @param currSize will be set to current buffer size
         * 
         * @returns bit information 0b'76543210'
         */
        uint8_t bufferOperation(const char *directText, const char *name, uint16_t playLength, uint32_t maxSize, uint32_t &currSize);

        uint32_t fourBeatTime;
        void stopAllMidi();
        midiHandler::channelMapEntry getMidiChannel(uint32_t channel);
        void readInstrument(addressableTextWalker *walker, midiHandler::channelMapEntry channel);

        playTrackInfo *getTrack(uint16_t track);
        void playTrack(uint16_t track);
        void playTrack(playTrackInfo *trackInfo);
        void playTrack(uint16_t track, playStack *stack);
        void playTrack(playTrackInfo *trackInfo, playStack *stack);

    }; /* class projektionMidi */

    inline bool otherMode(bool midiMode) { return !midiMode; }

} /* namespace projektionMidi */

#endif /* PROJEKTION_MIDI_PROJEKTION_MIDI_HPP_ */
