#include "envelope.h"
#include "defines.h"
#include <cassert>

ADAREnvelope::ADAREnvelope() {
  mStageSettings[0] = 1.0 / (0.2 * fm::fsample_rate()); //attack time
  mStageSettings[1] = 1.0 / (0.3 * fm::fsample_rate()); //decay time
}

float ADAREnvelope::compute() {
  float val = 0.0f;
  switch (mStage) {
    case COMPLETE:
      return 0.0f;
    case ATTACK:
      val = mPosition;
      mPosition += mStageSettings[ATTACK];
      if (mPosition >= 1.0f) {
        if (mMode == AD) {
          mStage = DECAY_RELEASE;
        } else {
          mPosition = 1;
        }
      }
      return val;
    case DECAY_RELEASE:
      val = mPosition;
      mPosition -= mStageSettings[DECAY_RELEASE];
      if (mPosition <= 0.0f) {
        mPosition = 0;
        mStage = COMPLETE;
        if (mCompleteCallback)
          mCompleteCallback();
      }
      return val;
  }
}

void ADAREnvelope::trigger(bool start) {
  if (start) {
    mPosition = 0;
    mStage = ATTACK;
  } else if (mMode == AR) {
    mStage = DECAY_RELEASE;
  }
}

void ADAREnvelope::stage_setting(stage_t stage, float v) {
  if (stage == COMPLETE)
    return;
  if (v <= 0.0)
    mStageSettings[stage] = 1.0;
  mStageSettings[stage] = 1.0 / (v * fm::fsample_rate());
}

ADAREnvelope::stage_t ADAREnvelope::stage() const { return mStage; }

void ADAREnvelope::mode(mode_t v) { mMode = v; }
ADAREnvelope::mode_t ADAREnvelope::mode() const { return mMode; }

void ADAREnvelope::complete_callback(complete_callback_t cb) {
  mCompleteCallback = cb;
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
  if (start) {
    mPosition = mValueLast; //so that we ramp up from where we were
    mStage = ATTACK;
  } else {
    mPosition = 0;
    mStage = RELEASE;
    mInterpMult = -mInterpPoint; //0 - start
  }
}

void ADSREnvelope::stage_setting(stage_t stage, float v) {
  if (stage == COMPLETE)
    return;
  if (stage != SUSTAIN) {
    if (v > 0.0)
      v = 1.0 / (v * fm::fsample_rate());
    else
      v = 1.0;
  }
  mStageSettings[stage] = v;
}

void ADSREnvelope::complete_callback(complete_callback_t cb) { mCompleteCallback = cb; }
ADSREnvelope::stage_t ADSREnvelope::stage() const { return mStage; }
