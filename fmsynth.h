#ifndef FMSYNTH_H
#define FMSYNTH_H

#include <array>
#include "fmvoice.h"

#define FM_VOICES 6

class FMSynth {
  public:
    float compute();
    void trigger(unsigned int voice, bool on, float frequency);
  private:
    std::array<FMVoice, FM_VOICES> mVoices;
};

#endif
