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
