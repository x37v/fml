#include "fmsynth.h"
#include "util.h"
#include <cassert>

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

namespace {
  //takes 10ms from 0..1
  const float volume_increment = 1.0f / (fm::fsample_rate() * 0.05);
  const float mod_freq_increment = 1.0f / (fm::fsample_rate() * 0.05);
  const float mod_depth_increment = 1.0f / (fm::fsample_rate() * 0.05);
}

FMSynth::FMSynth() {
  for (unsigned int i = 0; i < mVoices.size(); i++) {
    mVoices[i].complete_callback([this, i] (void) {
      if (mVoiceCompleteCallback)
        mVoiceCompleteCallback(i);
    });
  }
  slew_rate(0.0f);

  mModDepthIncrement = mod_depth_increment;
  mModFreqIncrement = mod_freq_increment;
  mVolumeIncrement = volume_increment;
}

float FMSynth::compute() {
  float out = 0;

  //smooth
  mModDepth = fm::lin_smooth(mModDepthTarget, mModDepth, mModDepthIncrement);
  mVolume = fm::lin_smooth(mVolumeTarget, mVolume, mVolumeIncrement);
  mModFreqOffset = fm::lin_smooth(mModFreqOffsetTarget, mModFreqOffset, mModFreqIncrement);

  for (auto& s: mVoices) {
    s.modulator_freq_offset(mModFreqOffset);
    s.mod_depth(mModDepth);
    out += s.compute();
  }
  return mVolume * (out / static_cast<float>(mVoices.size()));
}

void FMSynth::trigger(unsigned int voice, bool on, uint8_t midi_note, float velocity) {
  if (voice >= mVoices.size()) {
    assert(voice < mVoices.size());
    return;
  }

  if (mSlewNote == UINT8_MAX)
    mSlewNote = midi_note;

  //cout << "trig " << (on ? "on  " : "off ") << voice << " vel: " << velocity << endl;
  mVoices[voice].trigger(on, midi_note, velocity, (mSlewHeldOnly && mNotesDown == 0) ? midi_note : mSlewNote);

  if (mNotesDown == 0) {
    if (on) {
      mNotesDown++;
    }
  } else {
    mNotesDown += (on ? 1 : -1);
  }
  
  if (on) {
    mSlewNote = midi_note;
  }
}

void FMSynth::note(unsigned int voice, uint8_t midi_note) {
  if (voice >= mVoices.size()) {
    assert(false);
    return;
  }
  mVoices[voice].note(midi_note);
}

ADSR::envState FMSynth::volume_envelope_state(uint8_t voice) const {
  if (voice >= mVoices.size()) {
    assert(false);
    return ADSR::env_idle;
  }
  return mVoices[voice].volume_envelope_state();
}

void FMSynth::mode(FMVoice::mode_t v) {
  for (auto& s: mVoices)
    s.mode(v);
}

void FMSynth::feedback(float v) {
  if (v < -1)
    v = -1;
  else if (v > 1)
    v = 1;
  v = powf(v, 2.0);
  for (auto& s: mVoices)
    s.feedback(v);
}

void FMSynth::mod_depth(float v) {
  mModDepthTarget = 5.0 * powf(0.5 * v, 2.0);
  mModDepthIncrement = (mModDepthTarget > mModDepth) ? mod_depth_increment : -mod_depth_increment;
}

void FMSynth::freq_mult(float mod, float car) {
  for (auto& s: mVoices)
    s.freq_mult(mod, car);
}

void FMSynth::modulator_freq_offset(float v) {
  mModFreqOffsetTarget = v;
  mModFreqIncrement = (mModFreqOffsetTarget > mModFreqOffset) ? mod_freq_increment : -mod_freq_increment;
}

void FMSynth::slew_rate(float seconds_per_octave) {
  for (auto& s: mVoices)
    s.slew_rate(seconds_per_octave);
}

void FMSynth::volume(float v) {
  mVolumeTarget = v;
  mVolumeIncrement = (mVolumeTarget > mVolume) ? volume_increment : -volume_increment;
}

void FMSynth::volume_envelope_setting(ADSR::envState stage, float v) {
  for (auto& s: mVoices)
    s.volume_envelope_setting(stage, v);
}

void FMSynth::mod_envelope_setting(ADSR::envState stage, float v) {
  for (auto& s: mVoices)
    s.mod_envelope_setting(stage, v);
}

void FMSynth::complete_callback(voice_complete_cb_t cb) {
  mVoiceCompleteCallback = cb;
}

void FMSynth::all_off() {
  mNotesDown = 0;
  for (auto& s: mVoices)
    s.trigger(false, 64, 127.0);
}

void FMSynth::slew_held_only(bool v) {
  mSlewHeldOnly = v;
}
