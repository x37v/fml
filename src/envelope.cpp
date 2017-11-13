#include "envelope.h"
#include "defines.h"
#include <cassert>

namespace {
  float time_to_increment(float v) {
    if (v < 0.015)
      v = 0.015;
    return 1.0 / (fm::fsample_rate() * v);
  }
}

float Envelope::value() const { return mValue; }

float Envelope::compute() {
  switch (mStage) {
    case Stage::IDLE:
      break;
    case Stage::ATTACK:
      mValue += mAttackIncrement;
      if (mValue >= 1) {
        mValue = 1;
        mStage = (mMode == Mode::ATTACK_SUSTAIN_RELEASE) ? Stage::SUSTAIN : Stage::RELEASE;
      }
      break;
    case Stage::SUSTAIN:
      //wait for trigger off
      break;
    case Stage::RELEASE:
      mValue -= mReleaseIncrement;
      if (mValue <= 0) {
        mValue = 0;
        mStage = Stage::IDLE;
      }
      break;
  }
  return mValue;
}

void Envelope::trigger(bool on) {
  if (on) {
    mStage = Stage::ATTACK;
  } else if (mMode == Mode::ATTACK_SUSTAIN_RELEASE && mStage != Stage::IDLE) {
    mStage = Stage::RELEASE;
  }
}

void Envelope::attack_time(float v) { mAttackIncrement = time_to_increment(v); }

void Envelope::release_time(float v) { mReleaseIncrement = time_to_increment(v); }

void Envelope::set(TimeSetting s, float time) {
  switch (s) {
    case TimeSetting::ATTACK:
      attack_time(time);
      break;
    case TimeSetting::RELEASE:
      release_time(time);
      break;
  }
}

Envelope::Stage Envelope::stage() const { return mStage; }
Envelope::Mode Envelope::mode() const { return mMode; }
void Envelope::mode(Mode v) { mMode = v; }

bool Envelope::active() const { return !idle(); }
bool Envelope::idle() const { return mStage == Stage::IDLE; }
