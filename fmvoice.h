#ifndef FM_VOICE_HPP
#define FM_VOICE_HPP

class FMVoice {
  public:
    FMVoice();
    float compute();

  private:
    float mMPhase = 0.0f;
    float mCPhase = 0.0f;

    float mMPhaseInc = 0.0f;
    float mCPhaseInc = 0.0f;

    float mMOutLast = 0.0f;

    float mMFeedBack = 0.0f;
};

#endif
