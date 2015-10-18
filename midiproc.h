#ifndef MIDIPROC_H
#define MIDIPROC_H

#include "fmsynth.h"
#include "defines.h"
#include <deque>

class FMMidiProc {
  public:
    enum cc_map_t {
      RATIO = 8,
      FINE = 16,
      FBDK = 24,

      DEPTH = 9,
      VOL = 17,
      //PORT = 25,

      MOD_ENV_ATK = 14,
      MOD_ENV_DEC = 22,

      VOL_ENV_ATK = 15,
      VOL_ENV_REL = 23,
      //VOL_ENV_SUS = 31,
      //VOL_ENV_REL = 39,
    };

    FMMidiProc(FMSynth& synth);
    void process_cc(FMSynth& synth, uint8_t channel, uint8_t num, uint8_t val);
    void process_note(FMSynth& synth, bool on, uint8_t channel, uint8_t note, uint8_t vel);

    void voice_freed(unsigned int voice);
  private:
    std::deque<std::pair<uint8_t, uint8_t>> mNoteVoiceLRUQueue;
    std::deque<uint8_t> mFreeVoiceQueue;
    uint8_t mChannel = 0;
};

#endif
