#include "jackaudioio.hpp"
#include "fmsynth.h"
#include <chrono>
#include <thread>
#include <memory>
#include <signal.h>

bool should_exit = false;

void sighandler(int sig) { should_exit = true; }

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
          mTrigCount = 44100 * 2;
          mFM.trigger(0, true, 440);
        } else if (mTrigCount == 44100) {
          mFM.trigger(0, false, 440);
        }
      }
      return 0;
    }
    JackAudio() : JackCpp::AudioIO("fm", 0, 2) { }
  private:
    int mTrigCount = 44100 * 2;
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
