#include "midiproc.h"
#include <cmath>

/*
#include <cassert>
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
*/

namespace {
  const int num_ratios = 11;
}

FMMidiProc::FMMidiProc(FMSynth& synth) {
}

void FMMidiProc::process_cc(FMSynth& synth, uint8_t channel, uint8_t num, uint8_t val) {
  if (channel != mCCChannel)
    return;
  float fval = static_cast<float>(val) / 127.0;
  switch (num) {
    case TRANSPOSE:
      synth.transpose(12 * (((int16_t)val - 64) / 24));
      break;
    case MONO_MODE:
      synth.mono_mode(val != 0);
      break;
    case MOD_ENV_LINEAR:
      synth.mod_env_linear(val != 0);
      break;
    case SUSTAIN_PEDAL:
      synth.mod_envelope_mode(val < 64 ? Envelope::Mode::ATTACK_RELEASE : Envelope::Mode::ATTACK_SUSTAIN_RELEASE);
      break;

    case RATIO:
    case FINE:
    case FBDK:
    case DEPTH:
    case VOL:
    case SLEW:
    case MOD_ENV_ATK:
    case MOD_ENV_DEC:
    case VOL_ENV_ATK:
    case VOL_ENV_REL:
      process_float(synth, (cc_map_t)num, fval);
      break;

    default:
      break;
  }
}

void FMMidiProc::process_float(FMSynth& synth, cc_map_t mapping, float fval) {
  switch (mapping) {
    case RATIO:
      {
        int index = roundf((fval * 2.0 - 1.0) * static_cast<float>(num_ratios)); 
        if (std::abs(index) == num_ratios) {
          if (index > 0) {
            synth.mode(FMVoice::FIXED_MODULATOR);
          } else {
            synth.mode(FMVoice::FIXED_CARRIER);
          }
        } else {
          synth.mode(FMVoice::NORMAL);
          float mod = 1.0f, car = 1.0f;
          if (index > 0) {
            mod = 1.0 + index;
          } else if (index < 0) {
            car = 1.0 - index;
          }
          synth.freq_mult(mod, car);
        }
      }
      break;
    case FINE:
      synth.modulator_freq_offset(fval);
      break;
    case FBDK:
      synth.feedback(fval);
      break;
    case DEPTH:
      synth.mod_depth(fval);
      break;
    case VOL:
      synth.volume(fval);
      break;
    case SLEW:
      synth.slew_rate(fval * 0.25);
      break;

    case MOD_ENV_ATK:
      synth.mod_envelope_setting(Envelope::TimeSetting::ATTACK, 4.0 * fval);
      break;
    case MOD_ENV_DEC:
      {
        float v = 4.0 * fval;
        synth.mod_envelope_setting(Envelope::TimeSetting::RELEASE, v);
      }
      break;

    case VOL_ENV_ATK:
      synth.volume_envelope_setting(Envelope::TimeSetting::ATTACK, 0.015 + fval * 4.0);
      break;
    //case VOL_ENV_DEC:
      //synth.volume_envelope_setting(ADSR::env_decay, 0.015 + fval * 2.0);
      //break;
    //case VOL_ENV_SUS:
      //synth.volume_envelope_setting(ADSR::env_sustain, fval);
      //break;
    case VOL_ENV_REL:
      synth.volume_envelope_setting(Envelope::TimeSetting::RELEASE, 0.015 + fval * 10.0);
      break;
      /*
    case TRANSPOSE:
      synth.transpose(12 * (((int16_t)val - 64) / 24));
      break;
    case MONO_MODE:
      synth.mono_mode(val != 0);
      break;
    case MOD_ENV_LINEAR:
      synth.mod_env_linear(val != 0);
      break;
      */
    default:
      break;
  }
}

void FMMidiProc::process_bend(FMSynth& synth, uint8_t channel, uint8_t num0, uint8_t num1) {
  process_bend(synth, channel, ((uint16_t)num1 << 7) | num0);
}

void FMMidiProc::process_bend(FMSynth& synth, uint8_t channel, int v) {
  if (channel != mNoteChannel)
    return;
  float f = ((float)v - 8192.0f) / 8192.0f;
  synth.bend(f);
}

void FMMidiProc::process_note(FMSynth& synth, bool on, uint8_t channel, uint8_t note, uint8_t vel) {
  if (channel != mNoteChannel)
    return;
  synth.process_note(on, channel, note, vel);
}

