#include "fmvoice.h"
#include "defines.h"
#include "util.h"
#include <cmath>
#include <cstdlib>

//phase increment = frequency / sample_rate

namespace {
  const float velocity_increment = 1.0f / (fm::fsample_rate() * 0.005);
  const float offset_increment = 1.0f / (fm::fsample_rate() * 0.015);

  const float fixed_midi_start = -38.0f;
  const float fixed_midi_range = 123.0f - fixed_midi_start;

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

  mod_env_linear(false);

  /*
  mModEnv.stage_setting(ADSREnvelope::SUSTAIN, 0.1);
  mModEnv.stage_setting(ADSREnvelope::RELEASE, 0.1);
  mAmpEnv.mode(ADAREnvelope::AR);
  */

  mAmpEnv.setAttackRate(0.1 * 44100.0);
  mAmpEnv.setDecayRate(0.0 * 44100.0);
  mAmpEnv.setSustainLevel(1.0);
  mAmpEnv.setReleaseRate(1.0 * 44100.0);

  mModEnv.setAttackRate(fm::fsample_rate() * 0.01);
  mModEnv.setDecayRate(fm::fsample_rate() * 0.01);
  mModEnv.setReleaseRate(fm::fsample_rate() * 0.01);
  mModEnv.setSustainLevel(0.1);

  /*
   * XXX
  mAmpEnv.complete_callback([this] (void) {
    if (mCompleteCallback)
      mCompleteCallback();
  });
  */
}

void FMVoice::compute(float& left, float& right) {
  update_increments();

  float mod_env = mModEnv.process() * mModDepth * mModVelocity;
  float car_env = mAmpEnv.process() * mAmpVelocity;

  float mod = fm::sine(mMPhase + mMFeedBack * mMOutLast) * mod_env;
  float car = fm::sine(mCPhase + mod) * car_env;
  //float car2 = fm::sine(mCPhase2 + mod) * car_env;

  mAmpVelocity = fm::lin_smooth(mAmpVelocityTarget, mAmpVelocity, mAmpVelocityIncrement);
  mModVelocity = fm::lin_smooth(mModVelocityTarget, mModVelocity, mModVelocityIncrement);

  mMPhase = mMPhase + mMPhaseInc;
  mCPhase = mCPhase + mCPhaseInc;
  //mCPhase2 = mCPhase2 + (mCPhaseInc * 1.0001);
  while (mMPhase >= 1.0f)
    mMPhase -= 1.0f;
  while (mCPhase >= 1.0f)
    mCPhase -= 1.0f;
  //while (mCPhase2 >= 1.0f)
    //mCPhase2 -= 1.0f;
  while (mMPhase < 0.0f)
    mMPhase += 1.0f;
  while (mCPhase < 0.0f)
    mCPhase += 1.0f;
  //while (mCPhase2 < 0.0f)
    //mCPhase2 += 1.0f;
  mMOutLast = mod;

  left += car;
  right += car;
  //right += car2;
}

void FMVoice::trigger(bool on, uint8_t midi_note, float velocity, uint8_t slew_note) {
  bool retrigger = mAmpEnv.getState() == ADSR::env_idle || mAmpEnv.getState() == ADSR::env_release;

  if (on) {
    mMidiNote = slew_note;
    note(midi_note);

    mAmpVelocityTarget = remap_amp_velocity(velocity);
    mModVelocityTarget = remap_mod_velocity(velocity);
    mAmpVelocityIncrement = (mAmpVelocityTarget > mAmpVelocity) ? velocity_increment : -velocity_increment;
    mModVelocityIncrement = (mModVelocityTarget > mModVelocity) ? velocity_increment : -velocity_increment;
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

void FMVoice::note(uint8_t midi_note) {
  mMidiNoteTarget = midi_note;

  if (mSlewSecondsPerOctave > 0) {
    float diff = mMidiNoteTarget - mMidiNote;
    float sec = fabs(diff / 12.0) * mSlewSecondsPerOctave;
    if (sec > 0)
      mSlewIncrement = diff / (sec * fm::fsample_rate());
    else
      mSlewIncrement = 0;
  } else {
    mMidiNote = mMidiNoteTarget;
    mSlewIncrement = 0;
  }
}

void FMVoice::bend(float v) {
  mBend = v;
}

void FMVoice::transpose(float v) {
  mTranspose = v;
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
  mMFreqMultOffsetTarget = v;
  mMFreqMultOffsetIncrement = (mMFreqMultOffsetTarget > mMFreqMultOffset) ? offset_increment : -offset_increment;
}

void FMVoice::slew_rate(float seconds_per_octave) {
  mSlewSecondsPerOctave = seconds_per_octave;
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

ADSR::envState FMVoice::volume_envelope_state() const {
  return mAmpEnv.getState();
}

void FMVoice::mod_env_linear(bool v) {
  if (v) {
    mModEnv.setTargetRatioA(100);
    mModEnv.setTargetRatioDR(100);
  } else {
    mModEnv.setTargetRatioA(0.0001);
    mModEnv.setTargetRatioDR(0.0001);
  }
}

void FMVoice::mode(mode_t v) {
  mMode = v;
}

FMVoice::mode_t FMVoice::mode() const {
  return mMode;
}

void FMVoice::update_increments() {
  if (mSlewIncrement != 0)
    mMidiNote = fm::lin_smooth(mMidiNoteTarget, mMidiNote, mSlewIncrement);
  else
    mMidiNote = mMidiNoteTarget;

  float base_freq = fm::midi_note_to_freq(mMidiNote + mBend + mTranspose);

  mMFreqMultOffset = fm::lin_smooth(mMFreqMultOffsetTarget, mMFreqMultOffset, mMFreqMultOffsetIncrement);
  switch(mMode) {
    case FIXED_MODULATOR: 
      {
        float freq = fm::midi_note_to_freq((mMFreqMultOffset * fixed_midi_range + fixed_midi_start));
        mMPhaseInc = freq / fm::fsample_rate();
        mCPhaseInc = base_freq / fm::fsample_rate();
      }
      break;

    case FIXED_CARRIER:
      {
        float freq = fm::midi_note_to_freq((mMFreqMultOffset * fixed_midi_range + fixed_midi_start));
        mCPhaseInc = freq / fm::fsample_rate();
        mMPhaseInc = base_freq / fm::fsample_rate();
      }
      break;
    default:
      mMPhaseInc = (base_freq * (mMFreqMult + mMFreqMultOffset)) / fm::fsample_rate();
      mCPhaseInc = (base_freq * mCFreqMult) / fm::fsample_rate();
      break;
  }
}

