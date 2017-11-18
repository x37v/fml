#include "fmsynth.h"
#include "util.h"
#include <cassert>
#include <Arduino.h>

#include <cmath>

namespace {
  float volume_increment;
  float mod_freq_increment;
  float mod_depth_increment;
  float transpose_increment;
  float feedback_increment;
  float bend_increment;
}

FMSynth::FMSynth() {
  volume_increment = 1.0f / (fm::fsample_rate() * 0.2f);
  mod_freq_increment = 1.0f / (fm::fsample_rate() * 0.05f);
  mod_depth_increment = 1.0f / (fm::fsample_rate() * 0.15f);
  transpose_increment = 1.0f / (fm::fsample_rate() * 0.005f);
  feedback_increment = 1.0f / (fm::fsample_rate() * 0.2f);
  bend_increment = 1.0f / (fm::fsample_rate() * 0.015f);

  slew_rate(0.0f);

  mNotesDown.fill(0);
  mVoiceNotes.fill(0);
  mVoiceHistory.fill(0);

  mModDepthIncrement = mod_depth_increment;
  mModFreqIncrement = mod_freq_increment;
  mVolumeIncrement = volume_increment;
}

void FMSynth::compute(float * buffer, uint16_t length) {
  memset(buffer, 0, 2 * sizeof(float) * length);

  float *left = buffer;
  float *right = buffer + length;
  for (unsigned int i = 0; i < length; i++) {
    mModDepth = fm::lin_smooth(mModDepthTarget, mModDepth, mModDepthIncrement);
    mVolume = fm::lin_smooth(mVolumeTarget, mVolume, mVolumeIncrement);
    mModFreqOffset = fm::lin_smooth(mModFreqOffsetTarget, mModFreqOffset, mModFreqIncrement);
    mTranspose = fm::lin_smooth(mTransposeTarget, mTranspose, mTransposeIncrement);
    mFeedback = fm::lin_smooth(mFeedbackTarget, mFeedback, mFeedbackIncrement);
    mBend = fm::lin_smooth(mBendTarget, mBend, mBendIncrement);

    for (int v = 0; v < FM_VOICES; v++) {
      auto& s = mVoices[v];
      s.modulator_freq_offset(mModFreqOffset);
      s.mod_depth(mModDepth);
      s.transpose(mTranspose);
      s.feedback(mFeedback);
      s.bend(mBend);
      s.compute(1, left + i, right + i);
    }
    left[i] *= mVolume;
    right[i] *= mVolume;
  }
}

void FMSynth::trigger(unsigned int voice, bool on, uint8_t midi_note, float velocity) {
  if (voice >= mVoices.size()) {
    assert(voice < mVoices.size());
    return;
  }
  mVoiceHistory[voice] = mVoiceHistoryCounter++;
  mVoiceNotes[voice] = on ? midi_note : 255; //clear out the note if we're turning off so we don't find it when looking for on notes

  uint8_t voices_on = 0;
  for (auto v: mVoices) {
    if (!(v.volume_envelope_state() == Envelope::Stage::IDLE || v.volume_envelope_state() == Envelope::Stage::RELEASE))
      voices_on += 1;
  }

  if (mSlewNote == UINT8_MAX || (on && voices_on == 0 && mSlewHeldOnly)) {
    mSlewNote = midi_note;
  }

#if 0
  if (Serial) {
    Serial.print(on ? "trig on " : "trig off ");
    Serial.println(voice);
  }
#endif
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

Envelope::Stage FMSynth::volume_envelope_state(uint8_t voice) const {
  if (voice >= mVoices.size()) {
    assert(false);
    return Envelope::Stage::IDLE;
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
  mVolumeTarget = v / static_cast<float>(mVoices.size());
  mVolumeIncrement = (mVolumeTarget > mVolume) ? volume_increment : -volume_increment;
}

void FMSynth::volume_envelope_setting(Envelope::TimeSetting stage, float v) {
  v = fm::time_to_increment(v);
  for (auto& s: mVoices)
    s.volume_envelope_increment(stage, v);
}

void FMSynth::mod_envelope_setting(Envelope::TimeSetting stage, float v) {
  if (v > 0)
    v = fm::time_to_increment(v);
  else
    v = 1.0;
  for (auto& s: mVoices)
    s.mod_envelope_increment(stage, v);
}

void FMSynth::mod_envelope_mode(Envelope::Mode m) {
  for (auto& s: mVoices)
    s.mod_envelope_mode(m);
}

void FMSynth::mod_env_linear(bool v) {
  for (auto& s: mVoices)
    s.mod_env_linear(v);
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
  int voice = -1;
  if (vel == 0)
    on = false;

  //update the 'down' mask
  if (on) {
    mNotesDown[midi_note / 8] |= (1 << (midi_note % 8));
  } else {
    mNotesDown[midi_note / 8] &= ~(1 << (midi_note % 8));
  }

  //in mono mode, any key down or up just re-scans all down keys and picks out the highest
  if (mMonoMode) {
    voice = 0;
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
  } else {
    if (on) {
      //find an active voice or [shouldn't happen] a voice with the same current note
      //we can't break early because we need to see all notes to see if we have a match
      for (unsigned int i = 0; i < mVoices.size(); i++) {
        if (!mVoices[i].active() || mVoiceNotes[i] == midi_note) {
          voice = i;
        }
      }
      //find the least recently used if we don't have a free voice
      if (voice < 0) {
        voice = 0;
        for (unsigned int i = 1; i < FM_VOICES; i++) {
          if (mVoiceHistory[voice] > mVoiceHistory[i])
            voice = i;
        }
      }
    } else {
      for (unsigned int i = 0; i < mVoiceNotes.size(); i++) {
        if (mVoiceNotes[i] == midi_note) {
          voice = i;
          break;
        }
      }
    }
  }

  if (voice < 0)
    voice = 0;
  trigger(voice, on, midi_note, static_cast<float>(vel) / 127.0f);
}

