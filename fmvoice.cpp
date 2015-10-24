#include "fmvoice.h"
#include "defines.h"
#include <cmath>
#include <cstdlib>

//phase increment = frequency / sample_rate

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

  /*
  mModEnv.stage_setting(ADSREnvelope::SUSTAIN, 0.1);
  mModEnv.stage_setting(ADSREnvelope::RELEASE, 0.1);
  mAmpEnv.mode(ADAREnvelope::AR);
  */

  mAmpEnv.setAttackRate(0.1 * 44100.0);
  mAmpEnv.setDecayRate(0.0 * 44100.0);
  mAmpEnv.setSustainLevel(1.0);
  mAmpEnv.setReleaseRate(1.0 * 44100.0);

  mModEnv.setAttackRate(0.1 * 44100.0);
  mModEnv.setDecayRate(0.1 * 44100.0);
  mModEnv.setSustainLevel(0.1);
  mModEnv.setReleaseRate(10.0 * 44100.0);

  update_increments();

  mAmpEnv.complete_callback([this] (void) {
    if (mCompleteCallback)
      mCompleteCallback();
  });
}

float FMVoice::compute() {
  float mod_env = mModEnv.process() * mModDepth * mModVelocity;
  float car_env = mAmpEnv.process() * mAmpVelocity;

  float mod = sin(two_pi * mMPhase) * mod_env;
  float car = sin(two_pi * mCPhase) * car_env;

  mAmpVelocity = (3.0 * mAmpVelocity + mAmpVelocityTarget) / 4.0;
  mModVelocity = (3.0 * mModVelocity + mModVelocityTarget) / 4.0;

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
  bool idle = mAmpEnv.getState() == ADSR::env_idle;
  bool retrigger = idle || mAmpEnv.getState() == ADSR::env_release;
  if (on) {
    mBaseFreq = frequency;
    update_increments();
    //set a target amp velocity, we don't set it directly unless we're off,
    //otherwise we'll get clicks
    if (retrigger) {
      mAmpVelocityTarget = remap_amp_velocity(velocity);
      mModVelocityTarget = remap_mod_velocity(velocity);
      mAmpVelocity = mAmpVelocityTarget;
      mModVelocity = mModVelocityTarget;
    }
  }
  //don't trigger if we're already on
  if (!on) {
    mModEnv.gate(false);
    mAmpEnv.gate(false);
  } else if (retrigger) {
    mModEnv.gate(true);
    mAmpEnv.gate(true);
  }
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

void FMVoice::volume_envelope_setting(ADSR::envState stage, float v) {
  switch (stage) {
    case ADSR::env_attack:
      mAmpEnv.setAttackRate(fm::fsample_rate() * v);
      break;
    case ADSR::env_decay:
      mAmpEnv.setDecayRate(fm::fsample_rate() * v);
      break;
    case ADSR::env_sustain:
      mAmpEnv.setSustainLevel(v);
        break;
    case ADSR::env_release:
      mAmpEnv.setReleaseRate(fm::fsample_rate() * v);
        break;
    case ADSR::env_idle:
    default:
      break;
  }
}

void FMVoice::mod_envelope_setting(ADSR::envState stage, float v) {
  switch (stage) {
    case ADSR::env_attack:
      mModEnv.setAttackRate(fm::fsample_rate() * v);
      break;
    case ADSR::env_decay:
      mModEnv.setDecayRate(fm::fsample_rate() * v);
      break;
    case ADSR::env_sustain:
      mModEnv.setSustainLevel(v);
        break;
    case ADSR::env_release:
      mModEnv.setReleaseRate(fm::fsample_rate() * v);
        break;
    case ADSR::env_idle:
    default:
      break;
  }
}

void FMVoice::complete_callback(complete_callback_t cb) {
  mCompleteCallback = cb;
}

bool FMVoice::active() const {
  //return mAmpEnv.stage() != ADAREnvelope::COMPLETE;
  return mAmpEnv.getState() != ADSR::env_idle;
}

void FMVoice::update_increments() {
  mMPhaseInc = (mBaseFreq * (mMFreqMult + mMFreqMultOffset)) / fm::fsample_rate();
  mCPhaseInc = (mBaseFreq * mCFreqMult) / fm::fsample_rate();
}

