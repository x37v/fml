#pragma once

#include "AudioWrapper.h"
#include "src/fmsynth.h"
#include "src/midiproc.h"

class FMSynth;
class AudioFML : public xnor::AudioWrapperFloat<0, 2> {
  public:
    AudioFML();
    void process_cc(uint8_t channel, uint8_t num, uint8_t val);
    void process_bend(uint8_t channel, uint8_t num0, uint8_t num1);
    void process_note(bool on, uint8_t channel, uint8_t note, uint8_t vel);
  protected:
    virtual void compute(uint16_t nframes, std::array<float *, 0>& ins, std::array<float *, 2>& outs) override;
  private:
    FMSynth mSynth;
    FMMidiProc mMidiProc;
    float mData[2 * AUDIO_BLOCK_SAMPLES];
};

