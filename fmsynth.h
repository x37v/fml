#ifndef FMSYNTH_H
#define FMSYNTH_H

#include <array>
#include "fmvoice.h"
#include "defines.h"

class FMSynth {
  public:
    typedef std::function<void(unsigned int voice)> voice_complete_cb_t;

    FMSynth();

    float compute();
    void trigger(unsigned int voice, bool on, float frequency = 440); //frequency ignored for off

    void feedback(float v);
    void mod_depth(float v);
    void freq_mult(float mod, float car);

    void complete_callback(voice_complete_cb_t cb);

  private:
    std::array<FMVoice, FM_VOICES> mVoices;
    voice_complete_cb_t mVoiceCompleteCallback = nullptr;
};

#endif
