#ifndef FM_VOICE_HPP
#define FM_VOICE_HPP

#include "adsr.h"

class FMVoice {
  public:
    typedef std::function<void(void)> complete_callback_t;
    enum mode_t { NORMAL, FIXED_CARRIER, FIXED_MODULATOR };

    FMVoice();

    void compute(float& left, float& right);
    void trigger(bool on, uint8_t midi_note, float velocity, uint8_t slew_note = UINT8_MAX);
    void note(uint8_t midi_node);
    void bend(float v);
    void transpose(float v);

    void feedback(float v);
    void mod_depth(float v);
    void freq_mult(float mod, float car);
    void modulator_freq_offset(float v);

    void slew_rate(float seconds_per_octave);

    void volume_envelope_setting(ADSR::envState stage, float v);
    void mod_envelope_setting(ADSR::envState stage, float v);

    void complete_callback(complete_callback_t cb); 

    bool active() const;
    ADSR::envState volume_envelope_state() const;
    
    void mode(mode_t v);
    mode_t mode() const;
  private:
    void update_increments();

    complete_callback_t mCompleteCallback = nullptr;

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
    float mMFreqMultOffsetTarget = 0.0f;
    float mMFreqMultOffsetIncrement = 0.0f;

    float mSlewIncrement = 1.0f;
    float mSlewSecondsPerOctave = 0.0f;

    float mBend = 0.0f;
    float mTranspose = 0.0f;

    ADSR mModEnv;
    ADSR mAmpEnv;
};

#endif
