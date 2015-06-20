#include "jackaudioio.hpp"
#include "fmsynth.h"
#include <chrono>
#include <thread>
#include <memory>
#include <signal.h>

#include <random>

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
    virtual int audioCallback(jack_nframes_t nframes, 
        audioBufVector inBufs,
        audioBufVector outBufs){
      for (unsigned int j = 0; j < nframes; j++) {
        float v = mFM.compute() * mVolume;
        for (unsigned int i = 0; i < outBufs.size(); i++) {
          outBufs[i][j] = v;
        }
        mTrigCount--;
        if (mTrigCount <= 0) {
          mTrigCount = 44100;
          mFM.trigger(0, true, rand_note());
        } else if (mTrigCount == (44100 - 4100)) {
          mFM.trigger(0, false, 440);
        }
      }
      return 0;
    }
    JackAudio() : JackCpp::AudioIO("fm", 0, 2) { }
  private:
    int mTrigCount = 0;
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
