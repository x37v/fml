#ifndef FMADC_H
#define FMADC_H

class FMSynth;
namespace adc {
  void init();
  void process(FMSynth* synth);
}

#endif
