#include "midiproc.h"
#include <cmath>

namespace {
  float midi_note_to_freq(uint8_t midi_note) {
    return powf(2, (midi_note - 69.0) / 12.0) * 440;
  }
}

FMMidiProc::FMMidiProc() {
  for (int i = 0; i < FM_VOICES; i++)
    mFreeVoiceQueue.push_back(i);
}

void FMMidiProc::process_cc(FMSynth& synth, uint8_t channel, uint8_t num, uint8_t val) {
}

void FMMidiProc::process_note(FMSynth& synth, bool on, uint8_t channel, uint8_t note, uint8_t vel) {
  int voice = -1;
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
        break;
      }
    }
  }
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
