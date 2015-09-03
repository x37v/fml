#include "fmsynth.h"
#include <cassert>

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

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
  for (auto& s: mVoices)
    out += s.compute();
  return out / static_cast<float>(mVoices.size());
}

void FMSynth::trigger(unsigned int voice, bool on, float frequency, float velocity) {
  if (voice >= mVoices.size()) {
    assert(false);
    return;
  }
  //cout << "trig " << (on ? "on  " : "off ") << voice << " vel: " << velocity << endl;
  mVoices[voice].trigger(on, frequency, velocity);
}

void FMSynth::feedback(float v) {
  v *= 0.5;
  v *= v; // x**2
  if (v < -1)
    v = -1;
  else if (v > 1)
    v = 1;
  for (auto& s: mVoices)
    s.feedback(v);
}

void FMSynth::mod_depth(float v) {
  v = 5.0 * powf(0.5 * v, 2.0);
  for (auto& s: mVoices)
    s.mod_depth(v);
}

void FMSynth::freq_mult(float mod, float car) {
  for (auto& s: mVoices)
    s.freq_mult(mod, car);
}

void FMSynth::modulator_freq_offset(float v) {
  for (auto& s: mVoices)
    s.modulator_freq_offset(v);
}

void FMSynth::volume_envelope_setting(ADAREnvelope::stage_t stage, float v) {
  if (v <= 0)
    v = 0.000001;
  for (auto& s: mVoices)
    s.volume_envelope_setting(stage, v);
}

void FMSynth::mod_envelope_setting(ADSREnvelope::stage_t stage, float v) {
  if (v <= 0)
    v = 0.000001;
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
