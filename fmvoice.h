#ifndef FM_VOICE_HPP
#define FM_VOICE_HPP

#include "adsr.h"
#include "envelope.h"

class FMVoice {
  public:
    typedef std::function<void(void)> complete_callback_t;

    FMVoice();

    float compute();
    void trigger(bool on, float frequency, float velocity);

    void feedback(float v);
    void mod_depth(float v);
    void freq_mult(float mod, float car);
    void modulator_freq_offset(float v);

    void volume_envelope_setting(ADAREnvelope::stage_t stage, float v);
    void mod_envelope_setting(ADSREnvelope::stage_t stage, float v);

    void complete_callback(complete_callback_t cb); 

    bool active() const;
  private:
    void update_increments(); //based on frequency or ratio change

    complete_callback_t mCompleteCallback = nullptr;

    float mMPhase = 0.0f;
    float mCPhase = 0.0f;

    float mMPhaseInc = 0.0f;
    float mCPhaseInc = 0.0f;

    float mMOutLast = 0.0f;

    float mMFeedBack = 0.01f;
    float mModDepth = 0.1f;

    float mMFreqMult = 1.0f;
    float mCFreqMult = 1.0f;

    float mModVelocity = 1.0f;
    float mAmpVelocity = 1.0f;

    float mMFreqMultOffset = 0.0f;

    float mBaseFreq = 440.0f;

    ADSR mModEnv;
    ADSR mAmpEnv;
};

#endif
