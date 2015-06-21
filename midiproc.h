#ifndef MIDIPROC_H
#define MIDIPROC_H

#include "fmsynth.h"
#include "defines.h"
#include <deque>

class FMMidiProc {
  public:
    FMMidiProc();
    void process_cc(FMSynth& synth, uint8_t channel, uint8_t num, uint8_t val);
    void process_note(FMSynth& synth, bool on, uint8_t channel, uint8_t note, uint8_t vel);

    void voice_freed(unsigned int voice);
  private:
    std::deque<std::pair<uint8_t, uint8_t>> mNoteVoiceLRUQueue;
    std::deque<uint8_t> mFreeVoiceQueue;
};

#endif
