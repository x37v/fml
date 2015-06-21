#ifndef ENVELOPE_H
#define ENVELOPE_H

#include <array>

//all stages, except sustain, increment a counter from 0 to 1 and may do math
//against that to figure out the value

class ADEnvelope {
  public:
    ADEnvelope();
    float compute();
    void trigger();
    enum stage_t {ATTACK = 0, DECAY = 1, COMPLETE};

  private:
    stage_t mStage = COMPLETE;
    float mPosition = 0.0f;
    std::array<float, 2> mStageSettings;
};

class ADSREnvelope {
  public:
    ADSREnvelope();
    float compute();
    void trigger(bool start = true);
    enum stage_t {ATTACK = 0, DECAY = 1, SUSTAIN = 2, RELEASE = 3, COMPLETE};

  private:
    stage_t mStage = COMPLETE;
    float mPosition = 0.0f;
    std::array<float, 4> mStageSettings;
    float mValueLast = 0;
    float mInterpPoint = 1.0f; //y intercept, b, in line equation
    float mInterpMult = 1.0f; //the slope [m in y = mx + b], since we go 0..1 its always just y1 - y0
};

#endif
