#include "fmsynth.h"
#include "util.h"
#include <cassert>

#include <cmath>

namespace {
  //takes 10ms from 0..1
  const float volume_increment = 1.0f / (fm::fsample_rate() * 0.05);
  const float mod_freq_increment = 1.0f / (fm::fsample_rate() * 0.05);
  const float mod_depth_increment = 1.0f / (fm::fsample_rate() * 0.05);
  const float transpose_increment = 1.0f / (fm::fsample_rate() * 0.005);
  const float feedback_increment = 1.0f / (fm::fsample_rate() * 0.05);
  const float bend_increment = 1.0f / (fm::fsample_rate() * 0.015);
}

FMSynth::FMSynth() {
  for (unsigned int i = 0; i < mVoices.size(); i++) {
    /*
    mVoices[i].complete_callback([this, i] (void) {
      if (mVoiceCompleteCallback)
        mVoiceCompleteCallback(i);
    });
    mNoteLRUQueue.push_back({0, 0});
    */
  }
  slew_rate(0.0f);

  mNotesDown.fill(0);
  mModDepthIncrement = mod_depth_increment;
  mModFreqIncrement = mod_freq_increment;
  mVolumeIncrement = volume_increment;

  //complete_callback([this] (unsigned int voice) { voice_freed(voice); });
}

void FMSynth::compute(float * interleaved, uint16_t length) {
  //smooth
  mModDepth = fm::lin_smooth(mModDepthTarget, mModDepth, mModDepthIncrement);
  mVolume = fm::lin_smooth(mVolumeTarget, mVolume, mVolumeIncrement);
  mModFreqOffset = fm::lin_smooth(mModFreqOffsetTarget, mModFreqOffset, mModFreqIncrement);
  mTranspose = fm::lin_smooth(mTransposeTarget, mTranspose, mTransposeIncrement);
  mFeedback = fm::lin_smooth(mFeedbackTarget, mFeedback, mFeedbackIncrement);
  mBend = fm::lin_smooth(mBendTarget, mBend, mBendIncrement);

    for (auto& s: mVoices) {
      s.modulator_freq_offset(mModFreqOffset);
      s.mod_depth(mModDepth);
      s.transpose(mTranspose);
      s.feedback(mFeedback);
      s.bend(mBend);
    }

  float left, right;
  const float vol = mVolume / static_cast<float>(mVoices.size());
  for (uint16_t i = 0; i < length; i++) {
    left = right = 0;
    for (auto& s: mVoices) {
      s.compute(left, right);
    }
    left *= vol;
    right *= vol;
    interleaved[i] = left;
    interleaved[i + length] = right;
  }
}

void FMSynth::trigger(unsigned int voice, bool on, uint8_t midi_note, float velocity) {
  if (voice >= mVoices.size()) {
    assert(voice < mVoices.size());
    return;
  }

  uint8_t voices_on = 0;
  for (auto v: mVoices) {
    if (!(v.volume_envelope_state() == ADSR::env_idle || v.volume_envelope_state() == ADSR::env_release))
      voices_on += 1;
  }

  if (mSlewNote == UINT8_MAX || (on && voices_on == 0 && mSlewHeldOnly)) {
    mSlewNote = midi_note;
  }

  //cout << "trig " << (on ? "on  " : "off ") << voice << " vel: " << velocity << endl;
  mVoices[voice].trigger(on, midi_note, velocity, (mSlewHeldOnly && voices_on == 0) ? midi_note : mSlewNote);

  if (on) {
    if (mSlewFromFirstHeld) {
      if (voices_on == 0)
        mSlewNote = midi_note;
    } else {
      mSlewNote = midi_note;
    }
  }
}

void FMSynth::note(unsigned int voice, uint8_t midi_note) {
  if (voice >= mVoices.size()) {
    assert(false);
    return;
  }
  mVoices[voice].note(midi_note);
}

ADSR::envState FMSynth::volume_envelope_state(uint8_t voice) const {
  if (voice >= mVoices.size()) {
    assert(false);
    return ADSR::env_idle;
  }
  return mVoices[voice].volume_envelope_state();
}

void FMSynth::mode(FMVoice::mode_t v) {
  for (auto& s: mVoices)
    s.mode(v);
}

void FMSynth::bend(float v) {
  mBendTarget = v * 12.0;
  mBendIncrement = (mBendTarget > mBend) ? bend_increment : -bend_increment;
}

void FMSynth::transpose(int16_t midi_note) {
  mTransposeTarget = midi_note;
  mTransposeIncrement = (mTransposeTarget > mTranspose) ? transpose_increment : -transpose_increment;
}

void FMSynth::feedback(float v) {
  if (v < -1)
    v = -1;
  else if (v > 1)
    v = 1;
  v = powf(v, 2.0);
  mFeedbackTarget = v;
  mFeedbackIncrement = (mFeedbackTarget > mFeedback) ? feedback_increment : -feedback_increment;
}

void FMSynth::mod_depth(float v) {
  mModDepthTarget = 5.0 * powf(0.5 * v, 2.0);
  mModDepthIncrement = (mModDepthTarget > mModDepth) ? mod_depth_increment : -mod_depth_increment;
}

void FMSynth::freq_mult(float mod, float car) {
  for (auto& s: mVoices)
    s.freq_mult(mod, car);
}

void FMSynth::modulator_freq_offset(float v) {
  mModFreqOffsetTarget = v;
  mModFreqIncrement = (mModFreqOffsetTarget > mModFreqOffset) ? mod_freq_increment : -mod_freq_increment;
}

void FMSynth::slew_rate(float seconds_per_octave) {
  for (auto& s: mVoices)
    s.slew_rate(seconds_per_octave);
}

void FMSynth::volume(float v) {
  mVolumeTarget = v;
  mVolumeIncrement = (mVolumeTarget > mVolume) ? volume_increment : -volume_increment;
}

void FMSynth::volume_envelope_setting(ADSR::envState stage, float v) {
  for (auto& s: mVoices)
    s.volume_envelope_setting(stage, v);
}

void FMSynth::mod_envelope_setting(ADSR::envState stage, float v) {
  for (auto& s: mVoices)
    s.mod_envelope_setting(stage, v);
}

void FMSynth::mod_env_linear(bool v) {
  for (auto& s: mVoices)
    s.mod_env_linear(v);
}

void FMSynth::complete_callback(voice_complete_cb_t cb) {
  mVoiceCompleteCallback = cb;
}

void FMSynth::all_off() {
  for (auto& s: mVoices)
    s.trigger(false, 64, 127.0);
}

void FMSynth::slew_held_only(bool v) {
  mSlewHeldOnly = v;
}

void FMSynth::slew_from_first(bool v) {
  mSlewFromFirstHeld = v;
}

void FMSynth::mono_mode(bool v) {
  mMonoMode = v;
  slew_held_only(mMonoMode);
  slew_from_first(!mMonoMode);
}

void FMSynth::process_note(bool on, uint8_t channel, uint8_t midi_note, uint8_t vel) {
  //XXX
  trigger(0, on, midi_note, 1.0);
  return;

  //update the 'down' mask
  if (on) {
    mNotesDown[midi_note / 8] |= (1 << (midi_note % 8));
  } else {
    mNotesDown[midi_note / 8] &= ~(1 << (midi_note % 8));
  }

  //in mono mode, any key down or up just re-scans all down keys and picks out the highest
  if (mMonoMode) {
    bool found = false;
    for (int i = 15; i >= 0 && !found; i--) {
      for (int j = 7; j >= 0; j--) {
        if (mNotesDown[i] & (1 << j)) {
          found = on = true;
          midi_note = i * 8 + j;
          break;
        }
      }
    }
  }

#if 1
    if (on)
      note(0, midi_note);
    trigger(0, on, midi_note, 1.0);
#else
  if (on) {
    int voice = -1;
    if (mMonoMode) {
      voice = 0;
    } else {
      int same_note = -1;
      int release_note = -1;
      uint8_t lru_voice = 0;

      //look for a voice with the same note, a free voice, and the least recently used voice
      for (uint8_t i = 0; i < mNoteLRUQueue.size(); i++) {
        if (mNoteLRUQueue[i].first == midi_note) {
          same_note = i;
          break;
        }
        if (mNoteLRUQueue[i].second == 0)
          voice = i;
        if (volume_envelope_state(i) == ADSR::env_release)
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
    }

    mNoteLRUQueue[voice] = {midi_note, 1};
    auto vstate = volume_envelope_state(voice);
    if (mMonoMode && !((vstate == ADSR::env_idle || vstate == ADSR::env_release)))
      note(voice, midi_note);
    else
      trigger(voice, true, midi_note, static_cast<float>(vel) / 127.0f);
  } else {
    //don't actually take an 'off' note out of the queue because it needs its release time
    for (uint8_t i = 0; i < mNoteLRUQueue.size(); i++) {
      if (mNoteLRUQueue[i].first == midi_note) {
        trigger(i, false);
      }
    }
  }
#endif
}

void FMSynth::voice_freed(unsigned int voice) {
#if 0
  if (voice >= mNoteLRUQueue.size()) {
    assert(voice < mNoteLRUQueue.size());
    return;
  }
  //anything greater than our index should get decremented
  uint8_t index = mNoteLRUQueue[voice].second;
  if (index > 0) {
    mNoteLRUQueue[voice] = {0, 0};
    for (size_t i = 0; i < mNoteLRUQueue.size(); i++) {
      if (mNoteLRUQueue[i].second >= index)
        mNoteLRUQueue[i].second--;
    }
  }
#endif
}

