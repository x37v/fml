#ifndef FM_DEFINES_H
#define FM_DEFINES_H

#define FM_VOICES 2

#include <inttypes.h>
#include <cmath>

#ifdef __arm__
#include "arm_math.h"
#endif


namespace fm {
  const float two_pi = 6.283185307179586;
  inline unsigned int sample_rate() { return 44100; }
  inline float fsample_rate() { return 44100.0f; }
  float midi_note_to_freq(float midi_note);

  //float/offset 0..1
  //= sine(2 * pi * index + offset);
  inline float sine(float index, float offset = 0.0f) {
#ifdef __arm__
    return arm_sin_f32(index * two_pi + offset);
#else
    return std::sin(index * two_pi + offset);
#endif
  }
};

#endif
