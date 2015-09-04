#include "fmvoice.h"
#include "defines.h"
#include <cmath>
#include <cstdlib>

//phase increment = frequency / sampling_rate

namespace {
  const float two_pi = 6.28318530718f;

  float remap_amp_velocity(float velocity) {
    if (velocity < 0.5)
      return 0.8 * (2.0 * velocity); //0 -> 0, 0.5 -> 0.8, linearly
    return 0.4 * velocity + 0.6;
  }

  float remap_mod_velocity(float velocity) {
    if (velocity < 0.5)
      return velocity;
    if (velocity < 0.75)
      return 2.0 * velocity - 0.5;
    return 1.0;
  }
};

FMVoice::FMVoice() {
  mMPhase = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
  mCPhase = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

  mModEnv.stage_setting(ADSREnvelope::SUSTAIN, 0.1);
  mModEnv.stage_setting(ADSREnvelope::RELEASE, 0.1);
  mAmpEnv.mode(ADAREnvelope::AR);

  update_increments();

  mAmpEnv.complete_callback([this] (void) {
    if (mCompleteCallback)
      mCompleteCallback();
  });
}

float FMVoice::compute() {
  float mod_env = mModEnv.compute() * mModDepth * mModVelocity;
  float car_env = mAmpEnv.compute() * mAmpVelocity;

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

void FMVoice::trigger(bool on, float frequency, float velocity) {
  if (on) {
    mBaseFreq = frequency;
    update_increments();
    mAmpVelocity = remap_amp_velocity(velocity);
    mModVelocity = remap_mod_velocity(velocity);
  }
  mModEnv.trigger(on);
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

void FMVoice::volume_envelope_setting(ADAREnvelope::stage_t stage, float v) {
  mAmpEnv.stage_setting(stage, v);
}

void FMVoice::mod_envelope_setting(ADSREnvelope::stage_t stage, float v) {
  mModEnv.stage_setting(stage, v);
}

void FMVoice::complete_callback(complete_callback_t cb) {
  mCompleteCallback = cb;
}

bool FMVoice::active() const {
  return mAmpEnv.stage() != ADAREnvelope::COMPLETE;
}

void FMVoice::update_increments() {
  mMPhaseInc = (mBaseFreq * (mMFreqMult + mMFreqMultOffset)) / fm::fsample_rate();
  mCPhaseInc = (mBaseFreq * mCFreqMult) / fm::fsample_rate();
}

