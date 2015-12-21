#ifndef FMSYNTH_H
#define FMSYNTH_H

#include <array>
#include "fmvoice.h"
#include "defines.h"
#include "adsr.h"

class FMSynth {
  public:
    typedef std::function<void(unsigned int voice)> voice_complete_cb_t;

    FMSynth();

    float compute();
    void trigger(unsigned int voice, bool on,
        uint8_t midi_note = UINT8_MAX, float velocity = 1.0f); //frequency,velocity ignored for off
    void note(unsigned int voice, uint8_t note);

    ADSR::envState volume_envelope_state(uint8_t voice) const;

    void mode(FMVoice::mode_t v);

    void feedback(float v);
    void mod_depth(float v);
    void freq_mult(float mod, float car);
    void modulator_freq_offset(float v);

    void slew_rate(float seconds_per_octave);
    void volume(float v); //0..1

    void volume_envelope_setting(ADSR::envState stage, float v);
    void mod_envelope_setting(ADSR::envState stage, float v);

    void complete_callback(voice_complete_cb_t cb);

    void all_off();

  private:
    uint8_t mNotesDown = 0;
    float mModDepth = 0.0f;
    float mModDepthTarget = 0.0f;
    float mModDepthIncrement = 0.0f;
    float mModFreqOffset = 0.0f;
    float mModFreqOffsetTarget = 0.0f;
    float mModFreqIncrement = 0.0f;
    float mVolume = 1.0f;
    float mVolumeTarget = 1.0f;
    float mVolumeIncrement = 0.0f;
    uint8_t mSlewNote = UINT8_MAX;
    std::array<FMVoice, FM_VOICES> mVoices;
    voice_complete_cb_t mVoiceCompleteCallback = nullptr;
};

#endif
