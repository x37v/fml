#ifndef FM_DEFINES_H
#define FM_DEFINES_H

#define FM_VOICES 6

#include <inttypes.h>

namespace fm {
  unsigned int sample_rate();
  float fsample_rate();
  float midi_note_to_freq(uint8_t midi_note);
};

#endif
