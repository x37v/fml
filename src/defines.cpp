#include "defines.h"

namespace fm {
  float midi_note_to_freq(float midi_note) {
    return powf(2.0, (midi_note - 69.0) / 12.0) * 440.0;
  }
}

