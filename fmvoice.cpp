#include "fmvoice.h"
#include <cmath>

//phase increment = frequency / sampling_rate

namespace {
  const float two_pi = 6.28318530718f;
};

FMVoice::FMVoice() {
  mMPhaseInc = 440.0 / 44100.0f;
  mCPhaseInc = 440.0 / 44100.0f;
}

float FMVoice::compute() {
  float mod_env = 1.0f; //XXX tmp
  float car_env = 1.0f; //XXX tmp

  float mod = sin(two_pi * mMPhase) * mod_env;
  float car = sin(two_pi * mCPhase) * car_env;

  mMPhase = mMPhase + mMPhaseInc + mMFeedBack * mMOutLast;
  mCPhase = mCPhase + mCPhaseInc + mod;
  while (mMPhase >= 1.0f)
    mMPhase -= 1.0f;
  while (mCPhase >= 1.0f)
    mCPhase -= 1.0f;
  mMOutLast = mod;

  return car;
}
