#include "jackaudioio.hpp"
#include "fmvoice.h"
#include <chrono>
#include <thread>
#include <memory>

//just output fm voice
class JackAudio : public JackCpp::AudioIO {
  public:
    virtual int audioCallback(jack_nframes_t nframes, 
        audioBufVector inBufs,
        audioBufVector outBufs){
      for (unsigned int j = 0; j < nframes; j++) {
        float v = mFM.compute();
        for (unsigned int i = 0; i < outBufs.size(); i++) {
          outBufs[i][j] = v;
        }
      }
      return 0;
    }
    JackAudio() : JackCpp::AudioIO("fm", 0, 2) { }
  private:
    FMVoice mFM;
};

int main(int argc, char * argv[]) {
  JackAudio audio;
  audio.start();
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return 0;
}
