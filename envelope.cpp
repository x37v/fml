#include "envelope.h"
#include "defines.h"

ADEnvelope::ADEnvelope() {
  mStageSettings[0] = 1.0 / (1.50 * fm::fsample_rate()); //attack time
  mStageSettings[1] = 1.0 / (0.50 * fm::fsample_rate()); //decay time
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
  mStageSettings[ATTACK] = 1.0 / (0.03 * fm::fsample_rate()); //attack time
  mStageSettings[DECAY] = 1.0 / (0.05 * fm::fsample_rate()); //decay time
  mStageSettings[SUSTAIN] = 0.8; //sustain level
  mStageSettings[RELEASE] = 1.0 / (1.0 * fm::fsample_rate()); //release time
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
        mPosition = 0;
        mStage = COMPLETE;
      }
      break;
  }

  mValueLast = val;
  return val;
}

void ADSREnvelope::trigger(bool start) {
  mInterpPoint = mValueLast;
  if (start) {
    mStage = ATTACK;
    mPosition = 0;
  } else {
    mStage = RELEASE;
    mInterpMult = -mInterpPoint; //0 - start
  }
}

