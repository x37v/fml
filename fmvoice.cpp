#include "fmvoice.h"
#include "defines.h"
#include <cmath>
#include <cstdlib>

#include <iostream>
using std::cout;
using std::endl;

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

  mModEnv.setAttackRate(0);
  mModEnv.setDecayRate(4 * 44100.0);
  mModEnv.setSustainLevel(0.1);
  mModEnv.setReleaseRate(10.0 * 44100.0);

  mAmpEnv.complete_callback([this] (void) {
    if (mCompleteCallback)
      mCompleteCallback();
  });
}

float FMVoice::compute() {
  update_increments();

  float mod_env = mModEnv.process() * mModDepth * mModVelocity;
  float car_env = mAmpEnv.process() * mAmpVelocity;

  float mod = sin(two_pi * (mMPhase + mMFeedBack * mMOutLast)) * mod_env;
  float car = sin(two_pi * (mCPhase + mod)) * car_env;

  //XXX filtering is totally a guess
  mAmpVelocity = (99.0 * mAmpVelocity + mAmpVelocityTarget) / 100.0;
  mModVelocity = (99.0 * mModVelocity + mModVelocityTarget) / 100.0;

  mMPhase = mMPhase + mMPhaseInc;
  mCPhase = mCPhase + mCPhaseInc;
  while (mMPhase >= 1.0f)
    mMPhase -= 1.0f;
  while (mCPhase >= 1.0f)
    mCPhase -= 1.0f;
  while (mMPhase < 0.0f)
    mMPhase += 1.0f;
  while (mCPhase < 0.0f)
    mCPhase += 1.0f;
  mMOutLast = mod;

  //update the slew
  if (mCPhaseIncTarget != mCPhaseInc) {
    //mCPhaseInc = (999.0 * mCPhaseInc + mCPhaseIncTarget) / 1000.0;
    if (mCPhaseIncIncAdd) {
      mCPhaseInc += mCPhaseIncInc;
      if (mCPhaseInc > mCPhaseIncTarget)
        mCPhaseInc = mCPhaseIncTarget;
    } else {
      mCPhaseInc -= mCPhaseIncInc;
      if (mCPhaseInc < mCPhaseIncTarget)
        mCPhaseInc = mCPhaseIncTarget;
    }
  }

  return car;
}

void FMVoice::trigger(bool on, float freq, float velocity) {
  bool idle = mAmpEnv.getState() == ADSR::env_idle;
  bool retrigger = idle || mAmpEnv.getState() == ADSR::env_release;
  if (on) {
    frequency(freq);
    //set a target amp velocity, we don't set it directly unless we're off,
    //otherwise we'll get clicks
    if (retrigger) {
      mAmpVelocityTarget = remap_amp_velocity(velocity);
      mModVelocityTarget = remap_mod_velocity(velocity);
      //mAmpVelocity = mAmpVelocityTarget;
      //mModVelocity = mModVelocityTarget;
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

void FMVoice::frequency(float f) {
  mBaseFreq = f;
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
}

void FMVoice::modulator_freq_offset(float v) {
  mMFreqMultOffset = v;
}

void FMVoice::slew_increment(float v) {
  mCPhaseIncInc = v;
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

void FMVoice::mode(mode_t v) {
  mMode = v;
}

FMVoice::mode_t FMVoice::mode() const {
  return mMode;
}

void FMVoice::update_increments() {
 switch(mMode) {
  case FIXED_MODULATOR: 
    {
      float freq = fm::midi_note_to_freq((mMFreqMultOffset * 160.0 - 80.0));
      mMPhaseInc = freq / fm::fsample_rate();
      mCPhaseIncTarget = mBaseFreq / fm::fsample_rate();
    }
    break;

  case FIXED_CARRIER:
    {
      float freq = fm::midi_note_to_freq((mMFreqMultOffset * 160.0 - 80.0));
      mCPhaseIncTarget = freq / fm::fsample_rate();
      mMPhaseInc = mBaseFreq / fm::fsample_rate();
    }
    break;
  default:
    mMPhaseInc = (mBaseFreq * (mMFreqMult + mMFreqMultOffset)) / fm::fsample_rate();
    mCPhaseIncTarget = (mBaseFreq * mCFreqMult) / fm::fsample_rate();
    break;
 }

  //should our incrementing go up or down from current phase inc
  mCPhaseIncIncAdd = mCPhaseIncTarget > mCPhaseInc;
  if (mCPhaseIncInc <= 0)
    mCPhaseIncInc = 0.0000001;
}

