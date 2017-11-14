#ifndef FM_DEFINES_H
#define FM_DEFINES_H

#define FM_VOICES 5

#ifdef AUDIO_BLOCK_SAMPLES
#define FM_MAX_BUFFER_LENGTH AUDIO_BLOCK_SAMPLES
#else
#define FM_MAX_BUFFER_LENGTH 1024
#endif

#include <inttypes.h>
#include <cmath>

#ifdef __arm__
#include "arm_math.h"
#endif


namespace fm {
  const float two_pi = 6.28318530717958f;
#ifdef AUDIO_SAMPLE_RATE_EXACT
  constexpr float fsample_rate() { return AUDIO_SAMPLE_RATE_EXACT; }
#else
  constexpr float fsample_rate() { return 44100.0f; }
#endif
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
