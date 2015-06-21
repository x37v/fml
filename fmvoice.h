#ifndef FM_VOICE_HPP
#define FM_VOICE_HPP

#include "envelope.h"

class FMVoice {
  public:
    typedef std::function<void(void)> complete_callback_t;

    FMVoice();

    float compute();
    void trigger(bool on, float frequency);

    void feedback(float v);
    void mod_depth(float v);
    void freq_mult(float mod, float car);

    void complete_callback(complete_callback_t cb); 
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

    float mBaseFreq = 440.0f;

    ADEnvelope mModEnv;
    ADSREnvelope mAmpEnv;
};

#endif
