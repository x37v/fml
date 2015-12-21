#ifndef MIDIPROC_H
#define MIDIPROC_H

#include "fmsynth.h"
#include "defines.h"
#include <vector>

class FMMidiProc {
  public:
    enum cc_map_t {
      RATIO = 8,
      FINE = 16,
      FBDK = 24,

      DEPTH = 9,
      VOL = 17,
      SLEW = 25,

      MOD_ENV_ATK = 14,
      MOD_ENV_DEC = 22,

      VOL_ENV_ATK = 15,
      VOL_ENV_REL = 23,
      //VOL_ENV_SUS = 31,
      //VOL_ENV_DEC = 39,

      MONO_MODE = 50,
    };

    FMMidiProc(FMSynth& synth);
    void process_cc(FMSynth& synth, uint8_t channel, uint8_t num, uint8_t val);
    void process_note(FMSynth& synth, bool on, uint8_t channel, uint8_t note, uint8_t vel);

    void voice_freed(unsigned int voice);
  private:
    std::vector<std::pair<uint8_t, uint8_t>> mNoteLRUQueue;
    uint8_t mNoteChannel = 0;
    uint8_t mCCChannel = 1;

    bool mMonoMode = false;
    uint8_t mLastNote = 0;
};

#endif
