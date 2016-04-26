#ifndef ENVELOPE_H
#define ENVELOPE_H

#include <array>
#include <functional>

//all stages, except sustain, increment a counter from 0 to 1 and may do math
//against that to figure out the value

class ADAREnvelope {
  public:
    typedef std::function<void(void)> complete_callback_t;
    enum stage_t {ATTACK = 0, DECAY_RELEASE = 1, COMPLETE};
    enum mode_t { AD, AR };

    ADAREnvelope();
    float compute();
    void trigger(bool start = true);
    void stage_setting(stage_t stage, float v);

    void mode(mode_t v);
    mode_t mode() const;

    void complete_callback(complete_callback_t cb); 

    stage_t stage() const;
  private:
    complete_callback_t mCompleteCallback = nullptr;
    stage_t mStage = COMPLETE;
    mode_t mMode = AR;
    float mPosition = 0.0f;
    std::array<float, 2> mStageSettings;
};

class ADSREnvelope {
  public:
    typedef std::function<void(void)> complete_callback_t;
    enum stage_t {ATTACK = 0, DECAY = 1, SUSTAIN = 2, RELEASE = 3, COMPLETE};

    ADSREnvelope();
    float compute();
    void trigger(bool start = true);
    void stage_setting(stage_t stage, float v);

    void complete_callback(complete_callback_t cb); 

    stage_t stage() const;
  private:
    complete_callback_t mCompleteCallback = nullptr;

    stage_t mStage = COMPLETE;
    float mPosition = 0.0f;
    std::array<float, 4> mStageSettings;
    float mValueLast = 0;
    float mInterpPoint = 1.0f; //y intercept, b, in line equation
    float mInterpMult = 1.0f; //the slope [m in y = mx + b], since we go 0..1 its always just y1 - y0
};

#endif
