#include "midiproc.h"
#include <cmath>

#include <iostream>
using std::cout;
using std::endl;

namespace {
  float midi_note_to_freq(uint8_t midi_note) {
    return powf(2, (midi_note - 69.0) / 12.0) * 440;
  }
}

FMMidiProc::FMMidiProc(FMSynth& synth) {
  for (int i = 0; i < FM_VOICES; i++)
    mFreeVoiceQueue.push_back(i);
  synth.complete_callback([this] (unsigned int voice) { voice_freed(voice); });
}

void FMMidiProc::process_cc(FMSynth& synth, uint8_t channel, uint8_t num, uint8_t val) {
  if (channel != mChannel)
    return;
  float fval = static_cast<float>(val) / 127.0;
  switch (num) {
    case RATIO:
      {
        int index = (fval * 2.0 - 1.0) * 4; 
        if (index >= 0) {
          synth.freq_mult(index + 1.0, 1.0);
        } else {
          synth.freq_mult(1.0, -index);
        }
      }
      cout << "ratio" << endl;
      break;
    case FINE:
      synth.modulator_freq_offset(fval);
      cout << "fine" << endl;
      break;
    case FBDK:
      synth.feedback(fval);
      cout << "mod fbdk" << endl;
      break;
    case DEPTH:
      synth.mod_depth(fval);
      cout << "mod depth " << endl;
      break;

    case MOD_ENV_ATK:
      synth.mod_envelope_setting(ADSREnvelope::ATTACK, 2.0 * fval);
      cout << "mod atk " << endl;
      break;
    case MOD_ENV_DEC:
      synth.mod_envelope_setting(ADSREnvelope::DECAY, 2.0 * fval);
      cout << "mod dec " << endl;
      break;

    case VOL_ENV_ATK:
      synth.volume_envelope_setting(ADAREnvelope::ATTACK, 0.015 + fval * 2.0);
      cout << "vol attack " << endl;
      break;
    case VOL_ENV_DEC:
      synth.volume_envelope_setting(ADAREnvelope::DECAY_RELEASE, 0.015 + fval * 2.0);
      cout << "vol dec " << endl;
      break;
    //case VOL_ENV_SUS:
      //synth.volume_envelope_setting(ADSREnvelope::SUSTAIN, fval);
      //break;
    //case VOL_ENV_REL:
      //synth.volume_envelope_setting(ADSREnvelope::RELEASE, fval * 2.0);
      //break;
    default:
      break;
  }
}

void FMMidiProc::process_note(FMSynth& synth, bool on, uint8_t channel, uint8_t note, uint8_t vel) {
  if (channel != mChannel)
    return;
  int voice = -1;

  //XXX do we find sounding voices and dealloc them?
  
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
    synth.trigger(voice, true, midi_note_to_freq(note), static_cast<float>(vel) / 127.0f);
  } else {
    //don't actually take an 'off' note out of the queue because it needs its release time
    for (auto& nv: mNoteVoiceLRUQueue) {
      if (nv.first == note) {
        voice = nv.second;
        synth.trigger(voice, false);
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

