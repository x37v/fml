#include "fmsynth.h"
#include <cassert>

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

namespace {
  //takes 10ms from 0..1
  const float volume_increment = 1.0f / (fm::fsample_rate() * 0.01);
}

FMSynth::FMSynth() {
  for (unsigned int i = 0; i < mVoices.size(); i++) {
    mVoices[i].complete_callback([this, i] (void) {
      if (mVoiceCompleteCallback)
        mVoiceCompleteCallback(i);
    });
  }
}

float FMSynth::compute() {
  float out = 0;
  //XXX just a guess.. seems to work okay, the filter
  mModDepth = (mModDepth * 99.0 + mModDepthTarget) / 100.0;

  //smooth volume
  if (fabs(mVolumeTarget - mVolume) <= volume_increment) {
    mVolume = mVolumeTarget;
  } else {
    mVolume += (mVolumeTarget > mVolume) ? volume_increment : -volume_increment;
  }

  for (auto& s: mVoices) {
    s.mod_depth(mModDepth);
    out += s.compute();
  }
  return mVolume * (out / static_cast<float>(mVoices.size()));
}

void FMSynth::trigger(unsigned int voice, bool on, float frequency, float velocity) {
  if (voice >= mVoices.size()) {
    assert(false);
    return;
  }
  //cout << "trig " << (on ? "on  " : "off ") << voice << " vel: " << velocity << endl;
  mVoices[voice].trigger(on, frequency, velocity);
}

void FMSynth::frequency(unsigned int voice, float freq) {
  if (voice >= mVoices.size()) {
    assert(false);
    return;
  }
  mVoices[voice].frequency(freq);
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
  v = powf(v * 0.5, 2.0);
  for (auto& s: mVoices)
    s.feedback(v);
}

void FMSynth::mod_depth(float v) {
  mModDepthTarget = 5.0 * powf(0.5 * v, 2.0);
}

void FMSynth::freq_mult(float mod, float car) {
  for (auto& s: mVoices)
    s.freq_mult(mod, car);
}

void FMSynth::modulator_freq_offset(float v) {
  for (auto& s: mVoices)
    s.modulator_freq_offset(v);
}

void FMSynth::slew(float v) {
  float slew = powf(10.0, -(5.0 + 2.0 * v));

  for (auto& s: mVoices)
    s.slew_increment(slew);
}

void FMSynth::volume(float v) {
  mVolumeTarget = v;
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

void FMSynth::print_active() {
  bool any = false;
  for (int i = 0; i < mVoices.size(); i++) {
    if (mVoices[i].active()) {
      cout << i << "\t";
      any = true;
    }
  }
  if (any)
    cout << endl;
}
