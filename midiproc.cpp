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
  switch (num) {
    case RATIO:
      {
        int index = (static_cast<int>(val) - 64) / 2;
        if (index >= 0) {
          synth.freq_mult(index + 1.0, 1.0);
        } else {
          synth.freq_mult(1.0, -index);
        }
      }
      break;
    case FINE:
      synth.modulator_freq_offset(static_cast<float>(val) / 127.0);
      break;
    case FBDK:
      synth.feedback(static_cast<float>(val) / 127.0);
      break;
    case DEPTH:
      synth.mod_depth(static_cast<float>(val) / 127.0);
      break;
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
    synth.trigger(voice, true, midi_note_to_freq(note));
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

