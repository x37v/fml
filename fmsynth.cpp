#include "fmsynth.h"
#include <cassert>

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

