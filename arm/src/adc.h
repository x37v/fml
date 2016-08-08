#ifndef FMADC_H
#define FMADC_H

class FMSynth;
class FMMidiProc;
namespace adc {
  void init();
  void process(FMMidiProc& midiproc, FMSynth& synth);
}

#endif
