#include "midiproc.h"
#include <cmath>

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;

FMMidiProc::FMMidiProc(FMSynth& synth) {
  for (int i = 0; i < FM_VOICES; i++)
    mFreeVoiceQueue.push_back(i);
  synth.complete_callback([this] (unsigned int voice) { voice_freed(voice); });
}

void FMMidiProc::process_cc(FMSynth& synth, uint8_t channel, uint8_t num, uint8_t val) {
  if (channel != mCCChannel)
    return;
  float fval = static_cast<float>(val) / 127.0;
  switch (num) {
    case RATIO:
      {
        int index = roundf((fval * 2.0 - 1.0) * 4.0); 
        if (abs(index) == 4) {
          if (index > 0) {
            synth.mode(FMVoice::FIXED_MODULATOR);
            cout << "ratio: fixed modulator" << endl;
          } else {
            synth.mode(FMVoice::FIXED_CARRIER);
            cout << "ratio: fixed carrier" << endl;
          }
        } else {
          synth.mode(FMVoice::NORMAL);
          float mod = 1.0f, car = 1.0f;
          if (index >= 0) {
            mod = 1.0 + index;
          } else {
            car = -index;
          }
          synth.freq_mult(mod, car);
          cout << "ratio: mod: " << mod << std::setprecision(4) << " car: " << car << std::setprecision(4) << endl;
        }
      }
      break;
    case FINE:
      synth.modulator_freq_offset(fval);
      cout << "fine " << fval << endl;
      break;
    case FBDK:
      synth.feedback(fval);
      cout << "mod fbdk" << endl;
      break;
    case DEPTH:
      synth.mod_depth(fval);
      cout << "mod depth " << endl;
      break;
    case VOL:
      synth.volume(fval);
      cout << "volume" << endl;
      break;
    case SLEW:
      synth.slew(fval);
      cout << "slew" << endl;
      break;

    case MOD_ENV_ATK:
      synth.mod_envelope_setting(ADSR::env_attack, 2.0 * fval);
      cout << "mod atk " << endl;
      break;
    case MOD_ENV_DEC:
      synth.mod_envelope_setting(ADSR::env_decay, 2.0 * fval);
      cout << "mod dec " << endl;
      break;

    case VOL_ENV_ATK:
      synth.volume_envelope_setting(ADSR::env_attack, 0.015 + fval * 2.0);
      cout << "vol attack " << endl;
      break;
    //case VOL_ENV_DEC:
      //synth.volume_envelope_setting(ADSR::env_decay, 0.015 + fval * 2.0);
      //cout << "vol dec " << endl;
      //break;
    //case VOL_ENV_SUS:
      //synth.volume_envelope_setting(ADSR::env_sustain, fval);
      //break;
    case VOL_ENV_REL:
      synth.volume_envelope_setting(ADSR::env_release, 0.015 + fval * 4.0);
      cout << "vol release " << endl;
      break;
    default:
      break;
  }
}

void FMMidiProc::process_note(FMSynth& synth, bool on, uint8_t channel, uint8_t note, uint8_t vel) {
  if (channel != mNoteChannel)
    return;
  int voice = -1;

  //XXX do we find sounding voices and dealloc them?
  
  if (mMonoMode) {
    if (on) {
      float freq = fm::midi_note_to_freq(note);
      if (mLastNote == 255) //only true if we're off
        synth.trigger(0, true, freq, static_cast<float>(vel) / 127.0f);
      else
        synth.frequency(0,  freq);

      mLastNote = note;
    } else {
      if (mLastNote == note) {
        synth.trigger(0, false);
        mLastNote = 255;
      }
    }
  } else {
    if (on) {
      //use a free voice or the least recently used sounding voice
      if (mFreeVoiceQueue.size()) {
        voice = mFreeVoiceQueue.front();
        mFreeVoiceQueue.pop_front();
      } else {
        voice = mNoteVoiceLRUQueue.front().second;
        mNoteVoiceLRUQueue.pop_front();
        //XXX what to do about click?
      }
      mNoteVoiceLRUQueue.push_back({note, voice});
      synth.trigger(voice, true, fm::midi_note_to_freq(note), static_cast<float>(vel) / 127.0f);
    } else {
      //don't actually take an 'off' note out of the queue because it needs its release time
      for (auto& nv: mNoteVoiceLRUQueue) {
        if (nv.first == note) {
          voice = nv.second;
          synth.trigger(voice, false);
        }
      }
    }
  }

  //cout << voice << " note " << (on ? "on  " : "off ") << static_cast<int>(note) << endl;
}

void FMMidiProc::voice_freed(unsigned int voice) {
  for (int i = 0; i < mNoteVoiceLRUQueue.size(); i++) {
    if (mNoteVoiceLRUQueue[i].second == voice) {
      mFreeVoiceQueue.push_back(voice);
      mNoteVoiceLRUQueue.erase(mNoteVoiceLRUQueue.begin() + i);
      break;
    }
  }
}

