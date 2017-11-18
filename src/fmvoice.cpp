#include "fmvoice.h"
#include "defines.h"
#include "util.h"
#include <cmath>
#include <cstdlib>

#define DETUNED_STEREO

//phase increment = frequency / sample_rate

namespace {
  float velocity_increment = fm::fsample_period() / 0.005f;
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
}

FMVoice::FMVoice() :
  mModEnv(Envelope::Mode::ATTACK_RELEASE),
  mAmpEnv(Envelope::Mode::ATTACK_SUSTAIN_RELEASE)
{
  mMPhase = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
  mCPhase = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

  mod_env_linear(false);

  mAmpEnv.set(Envelope::TimeSetting::ATTACK, 0.1);
  mAmpEnv.set(Envelope::TimeSetting::RELEASE, 0.1);
  mAmpEnv.reset_on_trigger(false);

  mModEnv.set(Envelope::TimeSetting::ATTACK, 0.1);
  mModEnv.set(Envelope::TimeSetting::RELEASE, 0.1);
  mModEnv.reset_on_trigger(false);

  //XXX set mod sustain level to 0.1
}

void FMVoice::compute(unsigned int nframes, float* left, float* right) {
  for (unsigned int i = 0; i < nframes; i++) {
    update_increments();

    float mod_env = mModEnv.compute() * mModDepth * mModVelocity;
    float car_env = mAmpEnv.compute() * mAmpVelocity;

    float mod = fm::sine(mMPhase + mMFeedBack * mMOutLast) * mod_env;
    float car = fm::sine(mCPhase + mod) * car_env;
#ifdef DETUNED_STEREO
    float car2 = fm::sine(mCPhase2 + mod) * car_env;
#endif
    mMOutLast = mod;

    mAmpVelocity = fm::lin_smooth(mAmpVelocityTarget, mAmpVelocity, mAmpVelocityIncrement);
    mModVelocity = fm::lin_smooth(mModVelocityTarget, mModVelocity, mModVelocityIncrement);

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

#ifdef DETUNED_STEREO
    mCPhase2 = mCPhase2 + (mCPhaseInc * 1.0001);
    while (mCPhase2 >= 1.0f)
      mCPhase2 -= 1.0f;
    while (mCPhase2 < 0.0f)
      mCPhase2 += 1.0f;
    left[i] += car;
    right[i] += car2;
#else
    left[i] += car;
    right[i] += car;
#endif
  }
}

void FMVoice::trigger(bool on, uint8_t midi_note, float velocity, uint8_t slew_note) {
  if (on) {
    mMidiNote = slew_note;
    note(midi_note);

    mAmpVelocityTarget = remap_amp_velocity(velocity);
    mModVelocityTarget = remap_mod_velocity(velocity);
    mAmpVelocityIncrement = (mAmpVelocityTarget > mAmpVelocity) ? velocity_increment : -velocity_increment;
    mModVelocityIncrement = (mModVelocityTarget > mModVelocity) ? velocity_increment : -velocity_increment;
  }

  mAmpEnv.trigger(on);
  mModEnv.trigger(on);
}

void FMVoice::note(uint8_t midi_note) {
  mMidiNoteTarget = midi_note;

  if (mSlewSecondsPerOctave > 0) {
    float diff = mMidiNoteTarget - mMidiNote;
    float sec = fabsf(diff / 12.0f) * mSlewSecondsPerOctave;
    if (sec > 0)
      mSlewIncrement = diff * fm::fsample_period() / sec;
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
  mMFreqMultOffset = v;
}

void FMVoice::slew_rate(float seconds_per_octave) {
  mSlewSecondsPerOctave = seconds_per_octave;
}

void FMVoice::volume_envelope_setting(Envelope::TimeSetting s, float time) {
  mAmpEnv.set(s, time);
}

void FMVoice::mod_envelope_setting(Envelope::TimeSetting s, float time) {
  mModEnv.set(s, time);
}

bool FMVoice::active() const { return mAmpEnv.active(); }
bool FMVoice::idle() const { return mAmpEnv.idle(); }

Envelope::Stage FMVoice::volume_envelope_state() const { return mAmpEnv.stage(); }

void FMVoice::mod_env_linear(bool v) {
  //XXX DO IT
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
  switch(mMode) {
    case FIXED_MODULATOR: 
      {
        float freq = fm::midi_note_to_freq((mMFreqMultOffset * fixed_midi_range + fixed_midi_start));
        mMPhaseInc = freq * fm::fsample_period();
        mCPhaseInc = base_freq * fm::fsample_period();
      }
      break;

    case FIXED_CARRIER:
      {
        float freq = fm::midi_note_to_freq((mMFreqMultOffset * fixed_midi_range + fixed_midi_start));
        mCPhaseInc = freq * fm::fsample_period();
        mMPhaseInc = base_freq * fm::fsample_period();
      }
      break;
    default:
      mMPhaseInc = (base_freq * (mMFreqMult + mMFreqMultOffset)) * fm::fsample_period();
      mCPhaseInc = (base_freq * mCFreqMult) * fm::fsample_period();
      break;
  }
  if (mAmpEnv.idle())
    mModEnv.reset();
}

