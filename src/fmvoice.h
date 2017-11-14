#ifndef FM_VOICE_HPP
#define FM_VOICE_HPP

#include "envelope.h"
#include <inttypes.h>

class FMVoice {
  public:
    enum mode_t { NORMAL, FIXED_CARRIER, FIXED_MODULATOR };

    FMVoice();

    void compute(unsigned int nframes, float* left, float* right);
    void trigger(bool on, uint8_t midi_note, float velocity, uint8_t slew_note = UINT8_MAX);
    void note(uint8_t midi_node);
    void bend(float v);
    void transpose(float v);

    void feedback(float v);
    void mod_depth(float v);
    void freq_mult(float mod, float car);
    void modulator_freq_offset(float v);

    void slew_rate(float seconds_per_octave);

    void volume_envelope_setting(Envelope::TimeSetting s, float time);
    void mod_envelope_setting(Envelope::TimeSetting s, float time);

    bool idle() const;
    bool active() const;
    Envelope::Stage volume_envelope_state() const;

    void mod_env_linear(bool v);
    
    void mode(mode_t v);
    mode_t mode() const;
  private:
    void update_increments();

    mode_t mMode = NORMAL;

    float mMPhase = 0.0f;
    float mCPhase = 0.0f;
    float mCPhase2 = 0.0f;

    float mMPhaseInc = 0.0f;
    float mCPhaseInc = 0.0f;

    float mMidiNote = 0.0f;
    float mMidiNoteTarget = 0.0f;

    float mMOutLast = 0.0f;

    float mMFeedBack = 0.01f;
    float mModDepth = 0.1f;

    float mMFreqMult = 1.0f;
    float mCFreqMult = 1.0f;

    float mModVelocity = 1.0f;
    float mAmpVelocity = 1.0f;

    float mModVelocityTarget = 1.0f;
    float mAmpVelocityTarget = 1.0f;

    float mModVelocityIncrement = 0.0f;
    float mAmpVelocityIncrement = 0.0f;

    float mMFreqMultOffset = 0.0f;
    float mSlewIncrement = 1.0f;
    float mSlewSecondsPerOctave = 0.0f;

    float mBend = 0.0f;
    float mTranspose = 0.0f;

    Envelope mModEnv;
    Envelope mAmpEnv;
};

#endif
