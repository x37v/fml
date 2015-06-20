#ifndef FM_VOICE_HPP
#define FM_VOICE_HPP

#include "envelope.h"

class FMVoice {
  public:
    FMVoice();
    float compute();
    void trigger(bool on, float frequency);

  private:
    float mMPhase = 0.0f;
    float mCPhase = 0.0f;

    float mMPhaseInc = 0.0f;
    float mCPhaseInc = 0.0f;

    float mMOutLast = 0.0f;

    float mMFeedBack = 0.0f;
    float mModDepth = 0.1f;

    ADEnvelope mModEnv;
    ADSREnvelope mAmpEnv;
};

#endif
