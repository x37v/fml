#include "jackaudioio.hpp"
#include "jackmidiport.hpp"

#include "fmsynth.h"
#include "midiproc.h"
#include <chrono>
#include <thread>
#include <memory>
#include <signal.h>
#include <cmath>

#include <random>

#include <iostream>
using std::cout;
using std::endl;

bool should_exit = false;

void sighandler(int sig) { should_exit = true; }

float rand_note() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(220, 1000);

  return static_cast<float>(dis(gen));
}

//just output fm voice
class JackAudio : public JackCpp::AudioIO {
  public:
    virtual int audioCallback(jack_nframes_t nframes, audioBufVector inBufs, audioBufVector outBufs){

      void * midi_buff = mMidiInput.port_buffer(nframes);
      if (jack_nframes_t events = mMidiInput.event_count(midi_buff)) {
        for (uint32_t i = 0; i < events; i++) {
          jack_midi_event_t evt;
          if (mMidiInput.get(evt, midi_buff, i)) {
            if (evt.size == 3) {
              switch (JackCpp::MIDIPort::status(evt)) {
                case JackCpp::MIDIPort::NOTEON:
                  mMidiProc.process_note(mFM, true, JackCpp::MIDIPort::channel(evt), evt.buffer[1], evt.buffer[2]);
                  break;
                case JackCpp::MIDIPort::NOTEOFF:
                  mMidiProc.process_note(mFM, false, JackCpp::MIDIPort::channel(evt), evt.buffer[1], evt.buffer[2]);
                  break;
                case JackCpp::MIDIPort::CC:
                  mMidiProc.process_cc(mFM, JackCpp::MIDIPort::channel(evt), evt.buffer[1], evt.buffer[2]);
                  break;
              }
            }
          }
        }
      }

      for (unsigned int j = 0; j < nframes; j++) {
        float v = mFM.compute() * mVolume;
        for (unsigned int i = 0; i < outBufs.size(); i++) {
          outBufs[i][j] = v;
        }
        if (mActiveCounter-- <= 0) {
          mActiveCounter = 44100 * 2;
          //mFM.print_active();
        }
      }
      return 0;
    }
    JackAudio() : JackCpp::AudioIO("fm", 0, 2),
      mFM(),
      mMidiProc(mFM)
    { 
      mMidiInput.init(this, "fml_in");
      mFM.freq_mult(2, 1);
      mFM.feedback(0.001);
      mFM.mod_depth(0.03);
    }
  private:
    JackCpp::MIDIInPort mMidiInput;
    FMSynth mFM;
    FMMidiProc mMidiProc;
    float mVolume = 0.2;
    int mActiveCounter = 44100 * 2;
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
