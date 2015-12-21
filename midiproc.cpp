#include "midiproc.h"
#include <cmath>

#include <cassert>
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;

namespace {
  const int num_ratios = 11;
}

FMMidiProc::FMMidiProc(FMSynth& synth) {
  for (int i = 0; i < FM_VOICES; i++) {
    mNoteLRUQueue.push_back({0, 0});
  }
  synth.complete_callback([this] (unsigned int voice) { voice_freed(voice); });
}

void FMMidiProc::process_cc(FMSynth& synth, uint8_t channel, uint8_t num, uint8_t val) {
  if (channel != mCCChannel)
    return;
  float fval = static_cast<float>(val) / 127.0;
  switch (num) {
    case RATIO:
      {
        int index = roundf((fval * 2.0 - 1.0) * static_cast<float>(num_ratios)); 
        if (abs(index) == num_ratios) {
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
      synth.mod_envelope_setting(ADSR::env_attack, 0.01 + 2.0 * fval);
      break;
    case MOD_ENV_DEC:
      {
        float v = 2.0 * fval;
        synth.mod_envelope_setting(ADSR::env_decay, 0.01 + v);
        synth.mod_envelope_setting(ADSR::env_release, 0.01 + v);
      }
      break;

    case VOL_ENV_ATK:
      synth.volume_envelope_setting(ADSR::env_attack, 0.015 + fval * 2.0);
      break;
    //case VOL_ENV_DEC:
      //synth.volume_envelope_setting(ADSR::env_decay, 0.015 + fval * 2.0);
      //break;
    //case VOL_ENV_SUS:
      //synth.volume_envelope_setting(ADSR::env_sustain, fval);
      //break;
    case VOL_ENV_REL:
      synth.volume_envelope_setting(ADSR::env_release, 0.015 + fval * 10.0);
      break;
    case MONO_MODE:
      mMonoMode = (val != 0);
      synth.all_off();
    default:
      break;
  }
}

void FMMidiProc::process_note(FMSynth& synth, bool on, uint8_t channel, uint8_t note, uint8_t vel) {
  if (channel != mNoteChannel)
    return;

  if (mMonoMode) {
    if (on) {
      if (mLastNote == 255)
        synth.trigger(0, true, note, static_cast<float>(vel) / 127.0f);
      else
        synth.note(0, note);
      mLastNote = note;
    } else {
      if (mLastNote == note) {
        synth.trigger(0, false);
        mLastNote = 255;
      }
    }
  } else {
    if (on) {
      int voice = -1;
      int same_note = -1;
      int release_note = -1;
      uint8_t lru_voice = 0;

      //look for a voice with the same note, a free voice, and the least recently used voice
      for (uint8_t i = 0; i < mNoteLRUQueue.size(); i++) {
        if (mNoteLRUQueue[i].first == note) {
          same_note = i;
          break;
        }
        if (mNoteLRUQueue[i].second == 0)
          voice = i;
        if (synth.volume_envelope_state(i) == ADSR::env_release)
          release_note = i;
        if (mNoteLRUQueue[lru_voice].second < mNoteLRUQueue[i].second)
          lru_voice = i;
      }

      //same note, then free note, then release note, then least recently used
      if (same_note >= 0) {
        voice = same_note;
      } else if (voice < 0) {
        voice = release_note >= 0 ? release_note : lru_voice;
      }

      //increment all indicies that are non zero or the voice
      for (uint8_t i = 0; i < mNoteLRUQueue.size(); i++) {
        if (i == voice || mNoteLRUQueue[i].second == 0)
          continue;
        mNoteLRUQueue[i].second++;
      }

      mNoteLRUQueue[voice] = {note, 1};
      synth.trigger(voice, true, note, static_cast<float>(vel) / 127.0f);
    } else {
      //don't actually take an 'off' note out of the queue because it needs its release time
      for (uint8_t i = 0; i < mNoteLRUQueue.size(); i++) {
        if (mNoteLRUQueue[i].first == note) {
          synth.trigger(i, false);
        }
      }
    }
  }
}

void FMMidiProc::voice_freed(unsigned int voice) {
  if (voice >= mNoteLRUQueue.size()) {
    assert(voice < mNoteLRUQueue.size());
    return;
  }
  //anything greater than our index should get decremented
  uint8_t index = mNoteLRUQueue[voice].second;
  if (index > 0) {
    mNoteLRUQueue[voice] = {0, 0};
    for (int i = 0; i < mNoteLRUQueue.size(); i++) {
      if (mNoteLRUQueue[i].second >= index)
        mNoteLRUQueue[i].second--;
    }
  }
}

