#include "envelope.h"
#include "defines.h"
#include <cassert>

Envelope::Envelope(Mode m) : mMode(m) { }

float Envelope::value() const { return mValue; }

float Envelope::compute() {
  switch (mStage) {
    case Stage::IDLE:
      break;
    case Stage::ATTACK:
      if (mAttackIncrement <= 0) {
        mStage = mMode == Mode::ATTACK_SUSTAIN_RELEASE ? Stage::SUSTAIN : Stage::RELEASE;
      } else {
        mValue += mAttackIncrement;
        if (mValue >= 1) {
          mValue = 1;
          if (mMode == Mode::ATTACK_SUSTAIN_RELEASE) {
            mStage = Stage::SUSTAIN;
          } else {
            if (mReleaseIncrement >= 1) {
              mValue = 0;
              mStage = Stage::IDLE;
            } else {
              mStage = Stage::RELEASE;
            }
          }
        }
      }
      break;
    case Stage::SUSTAIN:
      //wait for trigger off
      break;
    case Stage::RELEASE:
      mValue -= mReleaseIncrement;
      if (mValue <= 0 || mReleaseIncrement <= 0) {
        mValue = 0;
        mStage = Stage::IDLE;
      }
      break;
  }
  return mValue;
}

void Envelope::reset() { 
  mStage = Stage::IDLE;
  mValue = 0;
}

void Envelope::trigger(bool on) {
  if (on) {
    mStage = Stage::ATTACK;
    if (mResetOnTrigger)
      mValue = 0;
  } else if (mMode == Mode::ATTACK_SUSTAIN_RELEASE && mStage != Stage::IDLE) {
    mStage = Stage::RELEASE;
  }
}

void Envelope::attack_time(float v) { mAttackIncrement = fm::time_to_increment(v); }

void Envelope::release_time(float v) { mReleaseIncrement = fm::time_to_increment(v); }

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

void Envelope::set_increment(TimeSetting s, float v) {
  switch (s) {
    case TimeSetting::ATTACK:
      mAttackIncrement = v;
      break;
    case TimeSetting::RELEASE:
      mReleaseIncrement = v;
      break;
  }
}

Envelope::Stage Envelope::stage() const { return mStage; }
Envelope::Mode Envelope::mode() const { return mMode; }
void Envelope::mode(Mode v) { mMode = v; }

bool Envelope::active() const { return !idle(); }
bool Envelope::idle() const { return mStage == Stage::IDLE; }
void Envelope::reset_on_trigger(bool v) { mResetOnTrigger = v; }
