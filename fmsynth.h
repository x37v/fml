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
    void trigger(unsigned int voice, bool on, float frequency = 440, float velocity = 1.0f); //frequency,velocity ignored for off
    void frequency(unsigned int voice, float frequency);

    void mode(FMVoice::mode_t v);

    void feedback(float v);
    void mod_depth(float v);
    void freq_mult(float mod, float car);
    void modulator_freq_offset(float v);

    void slew(float v); //0..1
    void volume(float v); //0..1

    void volume_envelope_setting(ADSR::envState stage, float v);
    void mod_envelope_setting(ADSR::envState stage, float v);

    void complete_callback(voice_complete_cb_t cb);

    //XXX debug
    void print_active();

  private:
    float mModDepth = 0.0f;
    float mModDepthTarget = 0.0f;
    float mModFreqOffset = 0.0f;
    float mModFreqOffsetTarget = 0.0f;
    float mVolume = 1.0f;
    float mVolumeTarget = 1.0f;
    std::array<FMVoice, FM_VOICES> mVoices;
    voice_complete_cb_t mVoiceCompleteCallback = nullptr;
};

#endif
