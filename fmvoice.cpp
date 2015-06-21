#include "fmvoice.h"
#include "defines.h"
#include <cmath>
#include <cstdlib>

//phase increment = frequency / sampling_rate

namespace {
  const float two_pi = 6.28318530718f;
};

FMVoice::FMVoice() {
  mMPhase = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
  mCPhase = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

  update_increments();

  mAmpEnv.complete_callback([this] (void) {
    if (mCompleteCallback)
      mCompleteCallback();
  });
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
  if (on) {
    mModEnv.trigger();
    mBaseFreq = frequency;
    update_increments();
  }
  mAmpEnv.trigger(on);
}

void FMVoice::feedback(float v) {
  mMFeedBack = v;
}

void FMVoice::mod_depth(float v) {
  mModDepth = v;
}

void FMVoice::freq_mult(float mod, float car) {
  mMFreqMult = mod;
  mCFreqMult = car;
  update_increments();
}

void FMVoice::modulator_freq_offset(float v) {
  mMFreqMultOffset = v;
  update_increments();
}

void FMVoice::complete_callback(complete_callback_t cb) {
  mCompleteCallback = cb;
}

bool FMVoice::active() const {
  return mAmpEnv.stage() != ADSREnvelope::COMPLETE;
}

void FMVoice::update_increments() {
  mMPhaseInc = (mBaseFreq * (mMFreqMult + mMFreqMultOffset)) / fm::fsample_rate();
  mCPhaseInc = (mBaseFreq * mCFreqMult) / fm::fsample_rate();
}

