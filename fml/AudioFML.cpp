#include <limits>
#include <inttypes.h>
#include "AudioFML.h"

AudioFML::AudioFML() : AudioWrapperFloat(), mSynth(), mMidiProc(mSynth) {
  mSynth.volume(0.95);
  mSynth.mod_depth(0.1);
  mSynth.mode(FMVoice::NORMAL);
  mSynth.volume_envelope_setting(Envelope::TimeSetting::RELEASE, 2.0);
  mSynth.mod_envelope_setting(Envelope::TimeSetting::ATTACK, 4.0);
  mSynth.mod_envelope_setting(Envelope::TimeSetting::RELEASE, 4.0);
  mSynth.freq_mult(1.0, 2.0);
  mSynth.modulator_freq_offset(0.1);
  mSynth.feedback(0.0);
}

void AudioFML::process_cc(uint8_t channel, uint8_t num, uint8_t val) {
  mMidiProc.process_cc(mSynth, channel, num, val);
}

void AudioFML::process_bend(uint8_t channel, uint8_t num0, uint8_t num1) {
  mMidiProc.process_bend(mSynth, channel, num0, num1);
}

void AudioFML::process_bend(uint8_t channel, int value) {
  mMidiProc.process_bend(mSynth, channel, value);
}

void AudioFML::process_note(bool on, uint8_t channel, uint8_t note, uint8_t vel) {
  mMidiProc.process_note(mSynth, on, channel, note, vel);
}

void AudioFML::compute(uint16_t nframes, std::array<float *, 0>& /*ins*/, std::array<float *, 2>& outs) {
  mSynth.compute(outs.front(), nframes);
}

#if 0
void AudioFML::update(void) {
  audio_block_t *left = allocate();
  audio_block_t *right = allocate();
  if (left == NULL || right == NULL) {
    if (left)
      release(left);
    if (right)
      release(right);
    return;
  }
  mSynth.compute(mData, AUDIO_BLOCK_SAMPLES);
  for (unsigned int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float l = mData[i * 2];
    float r = mData[i * 2 + 1];
    left->data[i] = std::numeric_limits<int16_t>::max() * l;
    right->data[i] = std::numeric_limits<int16_t>::max() * r;
    //block->data[i] = (i > (AUDIO_BLOCK_SAMPLES >> 1)) ? std::numeric_limits<int16_t>::max() : std::numeric_limits<int16_t>::min();
  }
  transmit(left, 0);
  transmit(right, 1);
  release(left);
  release(right);
}
#endif
