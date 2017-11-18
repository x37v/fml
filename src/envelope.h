#ifndef ENVELOPE_H
#define ENVELOPE_H

#include <array>
#include <functional>

class Envelope {
  public:
    enum class Stage { IDLE, ATTACK, SUSTAIN, RELEASE };
    enum class Mode { ATTACK_RELEASE, ATTACK_SUSTAIN_RELEASE };
    enum class TimeSetting { ATTACK, RELEASE };

    float value() const;
    float compute();
    void reset();

    void trigger(bool on);
    void attack_time(float v);
    void release_time(float v);
    void set(TimeSetting s, float time);

    Stage stage() const;
    Mode mode() const;
    void mode(Mode v);

    bool active() const;
    bool idle() const;
    void reset_on_trigger(bool v);
  private:
    bool mResetOnTrigger = false;
    float mValue = 0;
    float mAttackIncrement = 0.1;
    float mReleaseIncrement = 0.1;
    Stage mStage = Stage::IDLE;
    Mode mMode = Mode::ATTACK_RELEASE;
};

#endif
