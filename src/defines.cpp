#include "defines.h"
#include <cmath>

namespace fm {
  unsigned int sample_rate() { return 44100; }
  float fsample_rate() { return 44100.0f; }
  float midi_note_to_freq(float midi_note) {
    return powf(2.0, (midi_note - 69.0) / 12.0) * 440.0;
  }
}

