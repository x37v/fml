#include "envelope.h"
#include "defines.h"
#include <cassert>

ADEnvelope::ADEnvelope() {
  mStageSettings[0] = 1.0 / (0.2 * fm::fsample_rate()); //attack time
  mStageSettings[1] = 1.0 / (0.03 * fm::fsample_rate()); //decay time
}

float ADEnvelope::compute() {
  float val = 0.0f;
  switch (mStage) {
    case COMPLETE:
      return 0.0f;
    case ATTACK:
      val = mPosition;
      mPosition += mStageSettings[ATTACK];
      if (mPosition >= 1.0f) {
        mPosition = 0;
        mStage = DECAY;
      }
      return val;
    case DECAY:
      val = 1.0f - mPosition;
      mPosition += mStageSettings[DECAY];
      if (mPosition >= 1.0f) {
        mPosition = 0;
        mStage = COMPLETE;
      }
      return val;
  }
}

void ADEnvelope::trigger() {
  mPosition = 0;
  mStage = ATTACK;
}


ADSREnvelope::ADSREnvelope() {
  mStageSettings[ATTACK] = 1.0 / (0.04 * fm::fsample_rate()); //attack time
  mStageSettings[DECAY] = 1.0 / (0.04 * fm::fsample_rate()); //decay time
  mStageSettings[SUSTAIN] = 0.8; //sustain level
  mStageSettings[RELEASE] = 1.0 / (1.0 * fm::fsample_rate()); //release time

  for (int i = 0; i < mStageSettings.size(); i++) {
    if (mStageSettings[i] <= 0) {
      assert(false);
    }
  }
}

float ADSREnvelope::compute() {
  float val = 0.0;
  switch (mStage) {
    case COMPLETE:
      val = 0.0;
      break;
    case ATTACK:
      val = mPosition;
      mPosition += mStageSettings[ATTACK];
      if (mPosition >= 1.0f) {
        mPosition = 0;
        mStage = DECAY;
        mInterpPoint = val;
        mInterpMult = mStageSettings[SUSTAIN] - mInterpPoint;
      }
      break;
    case DECAY:
      val = mInterpPoint + mInterpMult * mPosition;
      mPosition += mStageSettings[DECAY];
      if (mPosition >= 1.0f) {
        mPosition = 0;
        mStage = SUSTAIN;
        mInterpPoint = val;
      }
      break;
    case SUSTAIN:
      val = mInterpPoint;
      break;
    case RELEASE:
      val = mInterpPoint + mInterpMult * mPosition;
      mPosition += mStageSettings[RELEASE];
      if (mPosition >= 1.0f) {
        val = 0.0f;
        mPosition = 0;
        mStage = COMPLETE;
        if (mCompleteCallback)
          mCompleteCallback();
      }
      break;
  }

  mValueLast = val;
  return val;
}

void ADSREnvelope::trigger(bool start) {
  mInterpPoint = mValueLast;
  mPosition = 0;
  if (start) {
    mStage = ATTACK;
  } else {
    mStage = RELEASE;
    mInterpMult = -mInterpPoint; //0 - start
  }
}

void ADSREnvelope::complete_callback(complete_callback_t cb) {
  mCompleteCallback = cb;
}

