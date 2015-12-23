#ifndef FMSYNTH_H
#define FMSYNTH_H

#include <array>
#include "fmvoice.h"
#include "defines.h"
#include "adsr.h"

#include <vector>
#include <inttypes.h>

class FMSynth {
  public:
    typedef std::function<void(unsigned int voice)> voice_complete_cb_t;

    FMSynth();

    void compute(float& left, float& right);
    void trigger(unsigned int voice, bool on,
        uint8_t midi_note = UINT8_MAX, float velocity = 1.0f); //frequency,velocity ignored for off
    void note(unsigned int voice, uint8_t midi_note);

    ADSR::envState volume_envelope_state(uint8_t voice) const;

    void mode(FMVoice::mode_t v);

    void bend(float v);
    void transpose(int16_t midi_note);

    void feedback(float v);
    void mod_depth(float v);
    void freq_mult(float mod, float car);
    void modulator_freq_offset(float v);

    void slew_rate(float seconds_per_octave);
    void volume(float v); //0..1

    void volume_envelope_setting(ADSR::envState stage, float v);
    void mod_envelope_setting(ADSR::envState stage, float v);
    void mod_env_linear(bool v);

    void complete_callback(voice_complete_cb_t cb);

    void all_off();
    //require a note to be held to slew?
    void slew_held_only(bool v);
    void slew_from_first(bool v);

    void mono_mode(bool v);

    void process_note(bool on, uint8_t channel, uint8_t note, uint8_t vel);
    void voice_freed(unsigned int voice);
  private:
    bool mMonoMode = false;
    float mModDepth = 0.0f;
    float mModDepthTarget = 0.0f;
    float mModDepthIncrement = 0.0f;
    float mModFreqOffset = 0.0f;
    float mModFreqOffsetTarget = 0.0f;
    float mModFreqIncrement = 0.0f;
    float mVolume = 1.0f;
    float mVolumeTarget = 1.0f;
    float mVolumeIncrement = 0.0f;
    float mTranspose = 0.0f;
    float mTransposeTarget = 0.0f;
    float mTransposeIncrement = 0.0f;
    float mBend = 0.0f;
    float mBendTarget = 0.0f;
    float mBendIncrement = 0.0f;
    bool mSlewHeldOnly = false;
    bool mSlewFromFirstHeld = true;
    uint8_t mSlewNote = UINT8_MAX;
    std::array<FMVoice, FM_VOICES> mVoices;
    voice_complete_cb_t mVoiceCompleteCallback = nullptr;
    std::array<uint8_t, 16> mNotesDown;

    std::vector<std::pair<uint8_t, uint8_t>> mNoteLRUQueue;
};

#endif
