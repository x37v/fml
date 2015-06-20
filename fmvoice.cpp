#include "fmvoice.h"
#include "defines.h"
#include <cmath>
#include <cstdlib>

//phase increment = frequency / sampling_rate

namespace {
  const float two_pi = 6.28318530718f;
};

FMVoice::FMVoice() {
  mMPhaseInc = 440.0 / fm::fsample_rate();
  mCPhaseInc = 440.0 / fm::fsample_rate();

  mMPhase = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
  mCPhase = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

float FMVoice::compute() {
  float mod_env = mModEnv.compute() * mModDepth;
  float car_env = mAmpEnv.compute();

  float mod = sin(two_pi * mMPhase) * mod_env;
  float car = sin(two_pi * mCPhase) * car_env;

  mMPhase = mMPhase + mMPhaseInc + mMFeedBack * mMOutLast;
  mCPhase = mCPhase + mCPhaseInc + mod;
  while (mMPhase >= 1.0f)
    mMPhase -= 1.0f;
  while (mCPhase >= 1.0f)
    mCPhase -= 1.0f;
  while (mMPhase < 0.0f)
    mMPhase += 1.0f;
  while (mCPhase < 0.0f)
    mCPhase += 1.0f;
  mMOutLast = mod;

  return car;
}

void FMVoice::trigger(bool on, float frequency) {
  if (on)
    mModEnv.trigger();
  mAmpEnv.trigger(on);
}

