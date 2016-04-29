//
//  ADRS.h
//
//  Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the ADSR envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//
//
//from http://www.earlevel.com/main/2013/06/03/envelope-generators-adsr-code/

#ifndef ADSR_H
#define ADSR_H

class ADSR {
  public:
    enum envState {
      env_idle = 0,
      env_attack,
      env_decay,
      env_sustain,
      env_release
    };

    //typedef std::function<void(void)> complete_callback_t;
    typedef void (*complete_callback_t)(void);
    ADSR();
    ~ADSR();
    float process();
    float getOutput();
    envState getState() const;
    void gate(bool on);
    void setAttackRate(float rate);
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
    void setSustainLevel(float level);
    void setTargetRatioA(float targetRatio);
    void setTargetRatioDR(float targetRatio);
    void reset();

    void complete_callback(complete_callback_t cb); 

  protected:
    envState state;
    float output;
    float attackRate;
    float decayRate;
    float releaseRate;
    float attackCoef;
    float decayCoef;
    float releaseCoef;
    float sustainLevel;
    float targetRatioA;
    float targetRatioDR;
    float attackBase;
    float decayBase;
    float releaseBase;

    float calcCoef(float rate, float targetRatio);
    complete_callback_t mCompleteCallback = nullptr;
};

inline float ADSR::process() {
  switch (state) {
    case env_idle:
      break;
    case env_attack:
      if (attackRate <= 0)
        output = 1.0;
      else
        output = attackBase + output * attackCoef;
      if (output >= 1.0) {
        output = 1.0;
        state = env_decay;
      }
      break;
    case env_decay:
      if (decayRate == 0) {
        state = env_sustain;
      } else {
        output = decayBase + output * decayCoef;
        if (output <= sustainLevel) {
          output = sustainLevel;
          state = env_sustain;
        }
      }
      break;
    case env_sustain:
      break;
    case env_release:
      output = releaseBase + output * releaseCoef;
      if (output <= 0.0) {
        output = 0.0;
        state = env_idle;
        if (mCompleteCallback)
          mCompleteCallback();
      }
  }
  return output;
}

inline void ADSR::gate(bool gate) {
  if (gate)
    state = env_attack;
  else if (state != env_idle)
    state = env_release;
}

inline ADSR::envState ADSR::getState() const {
  return state;
}

inline void ADSR::reset() {
  state = env_idle;
  output = 0.0;
}

inline float ADSR::getOutput() {
  return output;
}

#endif
