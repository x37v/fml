#include "fmsynth.h"
#include <cassert>

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

void FMSynth::trigger(unsigned int voice, bool on, float frequency) {
  if (voice >= mVoices.size()) {
    assert(false);
    return;
  }
  mVoices[voice].trigger(on, frequency);
}

void FMSynth::feedback(float v) {
  if (v < -1)
    v = -1;
  else if (v > 1)
    v = 1;
  for (auto& s: mVoices)
    s.feedback(v);
}

void FMSynth::mod_depth(float v) {
  for (auto& s: mVoices)
    s.mod_depth(v);
}

void FMSynth::freq_mult(float mod, float car) {
  for (auto& s: mVoices)
    s.freq_mult(mod, car);
}

void FMSynth::complete_callback(voice_complete_cb_t cb) {
  mVoiceCompleteCallback = cb;
}

