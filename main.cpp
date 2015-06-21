#include "jackaudioio.hpp"
#include "jackmidiport.hpp"

#include "fmsynth.h"
#include <chrono>
#include <thread>
#include <memory>
#include <signal.h>
#include <cmath>

#include <random>

bool should_exit = false;

void sighandler(int sig) { should_exit = true; }

float rand_note() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(220, 1000);

  return static_cast<float>(dis(gen));
}

float midi_to_freq(uint8_t midi_note) {
  return powf(2, (midi_note - 69.0) / 12.0) * 440;
}

//just output fm voice
class JackAudio : public JackCpp::AudioIO {
  public:
    virtual int audioCallback(jack_nframes_t nframes, audioBufVector inBufs, audioBufVector outBufs){

      void * midi_buff = mMidiInput.port_buffer(nframes);
      if (jack_nframes_t events = mMidiInput.event_count(midi_buff)) {
        for (uint32_t i = 0; i < events; i++) {
          jack_midi_event_t evt;
          if (mMidiInput.get(evt, midi_buff, i) && evt.size == 3) {
            switch (JackCpp::MIDIPort::status(evt)) {
              case JackCpp::MIDIPort::NOTEON:
                mFM.trigger(0, true, midi_to_freq(evt.buffer[1]));
                break;
              case JackCpp::MIDIPort::NOTEOFF:
                mFM.trigger(0, false, midi_to_freq(evt.buffer[1]));
                break;
            }
          }
        }
      }

      for (unsigned int j = 0; j < nframes; j++) {
        float v = mFM.compute() * mVolume;
        for (unsigned int i = 0; i < outBufs.size(); i++) {
          outBufs[i][j] = v;
        }
      }
      return 0;
    }
    JackAudio() : JackCpp::AudioIO("fm", 0, 2) { 
      mMidiInput.init(this, "fml_in");
      mFM.freq_mult(8, 1);
      mFM.feedback(0);
      mFM.mod_depth(0.3);
    }
  private:
    JackCpp::MIDIInPort mMidiInput;
    FMSynth mFM;
    float mVolume = 0.2;
};

int main(int argc, char * argv[]) {
  signal(SIGINT, &sighandler);
  
  JackAudio audio;
  audio.start();

  audio.connectToPhysical(0,0);
  audio.connectToPhysical(1,1);

  while (!should_exit) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  audio.stop();
  return 0;
}
