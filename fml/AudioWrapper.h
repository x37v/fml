#pragma once

#include <array>
#include <limits>
#include <inttypes.h>
#include <AudioStream.h>
#include <arm_math.h>

namespace xnor {
  float sampleToFloat(const int16_t& v);
  int16_t floatToSample(const float& v);

  template <uint8_t nInputs, uint8_t nOutputs> 
    class AudioWrapper : public AudioStream {
      public:
        AudioWrapper() : AudioStream(nInputs, &mInputBlocks.front()) {
          for (unsigned int i = 0; i < mReceivedBlocks.size(); i++)
            mReceivedBlocks[i] = NULL;
          for (unsigned int i = 0; i < mOutputBlocks.size(); i++)
            mOutputBlocks[i] = NULL;
        }

      protected:
        // overload to process audio data, nframes of samples
        // NOTE: ins data should be treated as read only
        // any value can be null in the input, treat it as a nframes array of silence
        virtual void compute(uint16_t nframes, const std::array<int16_t *, nInputs>& ins, std::array<int16_t *, nOutputs>& outs) = 0;

      public:
        virtual void update(void) override {
          if (!allocBlocks()) {
            for (unsigned int i = 0; i < mOutputBlocks.size(); i++) {
              if (mOutputBlocks[i] == NULL)
                continue;
              release(mOutputBlocks[i]);
              mOutputBlocks[i] = NULL;
            }
          } else {
            std::array<int16_t *, nInputs> inputs;
            std::array<int16_t *, nOutputs> outputs;
            for (unsigned int i = 0; i < nInputs; i++)
              inputs[i] = mReceivedBlocks[i] == NULL ? nullptr : mReceivedBlocks[i]->data;
            for (unsigned int i = 0; i < nOutputs; i++)
              outputs[i] = mOutputBlocks[i]->data;

            compute(AUDIO_BLOCK_SAMPLES, inputs, outputs);

            //transmit!
            for (unsigned int i = 0; i < nOutputs; i++) {
              transmit(mOutputBlocks[i], i);
              release(mOutputBlocks[i]);
              mOutputBlocks[i] = NULL;
            }
          }
          for (unsigned int i = 0; i < mReceivedBlocks.size(); i++) {
            if (mReceivedBlocks[i] == NULL)
              continue;
            release(mReceivedBlocks[i]);
            mReceivedBlocks[i] = NULL;
          }
        }

      private:
        std::array<audio_block_t *, nInputs> mInputBlocks; //pass along and forget
        std::array<audio_block_t *, nInputs> mReceivedBlocks;
        std::array<audio_block_t *, nOutputs> mOutputBlocks;

        bool allocBlocks() {
          // receive input blocks, can be null, treat as all zero
          for (unsigned int i = 0; i < mReceivedBlocks.size(); i++)
            mReceivedBlocks[i] = receiveReadOnly(i);

          // alloc output blocks, abort if we can't get all that we need
          for (unsigned int i = 0; i < mOutputBlocks.size(); i++) {
            auto a = mOutputBlocks[i] = allocate();
            if (a == NULL)
              return false;
          }
          return true;
        }
    };

  template <uint8_t nInputs, uint8_t nOutputs> 
    class AudioWrapperFloat : public AudioWrapper<nInputs, nOutputs> {
      public:
        AudioWrapperFloat() : AudioWrapper<nInputs, nOutputs>() { }

      protected:
        // overload to process audio data, ins can be overwritten as needed
        virtual void compute(uint16_t nframes, std::array<float *, nInputs>& ins, std::array<float *, nOutputs>& outs) = 0;

        virtual void compute(uint16_t nframes, const std::array<int16_t *, nInputs>& ins, std::array<int16_t *, nOutputs>& outs) override {
          std::array<float, nInputs * AUDIO_BLOCK_SAMPLES> receivedSamples;
          std::array<float, nOutputs * AUDIO_BLOCK_SAMPLES> outputSamples;
          std::array<float *, nInputs> fins;
          std::array<float *, nOutputs> fouts;

          // convert input to interleaved floating point
          float * p = &receivedSamples.front();
          for (unsigned int i = 0; i < ins.size(); i++) {
            if (ins[0] == nullptr) {
              for (unsigned int j = 0; j < AUDIO_BLOCK_SAMPLES; j++)
                *(p++) = 0;
            } else {
              for (unsigned int j = 0; j < AUDIO_BLOCK_SAMPLES; j++) 
                *(p++) = sampleToFloat(ins[i][j]);
            }
            fins[i] = &receivedSamples.front() + i * AUDIO_BLOCK_SAMPLES;
          }
          for (unsigned int i = 0; i < outs.size(); i++)
            fouts[i] = &outputSamples.front() + i * AUDIO_BLOCK_SAMPLES;

          compute(AUDIO_BLOCK_SAMPLES, fins, fouts);

          // convert output from interleaved floating point
          float * o = &outputSamples.front();
#if 1
          for (unsigned int i = 0; i < outs.size(); i++) {
            for (unsigned int j = 0; j < AUDIO_BLOCK_SAMPLES; j++)
              outs[i][j] = floatToSample(*(o++));
          }
#else
          for (unsigned int i = 0; i < outs.size(); i++) {
            arm_float_to_q15(o, outs[i], AUDIO_BLOCK_SAMPLES);
            o += AUDIO_BLOCK_SAMPLES;
          }
#endif
        }
    };
}
