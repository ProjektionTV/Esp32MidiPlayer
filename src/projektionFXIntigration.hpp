#ifndef PROJEKTIONFX_INTIGRATION_HPP_
#define PROJEKTIONFX_INTIGRATION_HPP_

#include "core/fxHandler.hpp"
#include "settings.h"

#define NUM_LEDS PROJEKTION_FX_LEDS

#include "BeatInfo.h"
#include "effects.h"
#include "e131sync.h"

CRGBArray<NUM_LEDS> leds;
BeatInfo beatInfo;

class ProjektionFX_FXHandler : public projektionMidi::FXHandler {
    public:
    void tick(uint64_t us) {
        if(stoped) return;
        unsigned long thisMs = millis();
        if(lastMs == 0) lastMs = thisMs;
        // limit to 30fps due to e131 is limited to 30fps
        // and we don't want to waste time rendering unused frames
        if(thisMs - lastMs >= (1000 / 30)) {
            lastMs = thisMs;
            beatInfo.loop();
            effectsRunner.run();
            e131sync.loop();
            EVERY_N_BSECONDS(30) {
                effectsRunner.nextEffect();
            }
        }
    }
    void setBpm(uint32_t bpm) {
        beatInfo.setBPM(bpm);
    }
    void start() {
        stoped = false;
    }
    void stop() {
        stoped = true;
    }
    ProjektionFX_FXHandler() {
        e131sync.setup();
    }
    private:
    unsigned long lastMs = 0;
    bool stoped = true;
};

ProjektionFX_FXHandler projektionFxHandler;

#endif /* PROJEKTIONFX_INTIGRATION_HPP_ */
