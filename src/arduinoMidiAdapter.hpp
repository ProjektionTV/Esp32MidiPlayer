#ifndef ARDUINO_MIDI_ADAPTER_HPP_
#define ARDUINO_MIDI_ADAPTER_HPP_

#include <MIDI.h>
#include "core/midiHandler.hpp"
#include "core/projektionMidi.hpp"

template<class MidiTransport>
class arduinoMidiAdapter {
    public:
    arduinoMidiAdapter(MIDI_NAMESPACE::MidiInterface<MidiTransport> *interface, uint8_t channelCount):
            interface(interface),
            channelCount(channelCount),
            low((uint64_t *) malloc(sizeof(uint64_t) * channelCount)),
            high((uint64_t *) malloc(sizeof(uint64_t) * channelCount)) {};
    ~arduinoMidiAdapter() { free(low); free(high); }

    void toggle(uint8_t channel, uint8_t note, uint8_t velocity) {
        uint64_t inType = note - (note >= 64 ? 64 : 0);
        uint64_t *list = note >= 64 ? high : low;
        if(list[channel - 1] & (1 << inType)) {
            interface->sendNoteOff(note, 0, channel);
        } else {
            interface->sendNoteOn(note, velocity, channel);
        }
        list[channel - 1] ^= 1 << inType;
    }
    void off(uint8_t channel) {
        for(uint8_t i = 0; i < 64; i++)
            if(low[channel - 1] & (1 << i))
                interface->sendNoteOff(i, 0, channel);
        low[channel - 1] = 0;
        for(uint8_t i = 0; i < 64; i++)
            if(high[channel - 1] & (1 << i))
                interface->sendNoteOff(i + 64, 0, channel);
        high[channel - 1] = 0;
    }
    void program(uint8_t channel, uint8_t program) {
        interface->sendProgramChange(program, channel);
    }
    void control(uint8_t channel, uint8_t control, uint8_t value) {
        interface->sendControlChange(control, value, channel);
    }

    void apply(projektionMidi::projektionMidi *midi, uint8_t textBaseChannel) {
        for(uint8_t i = channelCount ; i > 0; i--) {
            midi->addMidiChannel(textBaseChannel + i - 1, &midiHandler, i);
        }
    }

    private:
    MIDI_NAMESPACE::MidiInterface<MidiTransport> *interface;
    uint8_t channelCount;
    uint64_t *low;
    uint64_t *high;
    projektionMidi::midiHandler::eventHandler midiHandler {
        .note = [this](uint8_t channel, uint8_t note, uint8_t velocity) { this->toggle(channel, note, velocity); },
        .allOff = [this](uint8_t channel) { this->off(channel); },
        .programChange = [this](uint8_t channel, uint8_t program) { this->program(channel, program); },
        .controlChange = [this](uint8_t channel, uint8_t control, uint8_t value) { this->control(channel, control, value); }
    };
}; /* class arduinoMidiAdapter */
template<class midiInt>
arduinoMidiAdapter(midiInt *, uint8_t channelCount) -> arduinoMidiAdapter<std::remove_reference_t<decltype(*std::declval<midiInt>().getTransport())>>;

#endif /* ARDUINO_MIDI_ADAPTER_HPP_ */
