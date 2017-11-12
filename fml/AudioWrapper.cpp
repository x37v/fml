#include "AudioWrapper.h"

namespace xnor {
  float sampleToFloat(const int16_t& v) {
    return static_cast<float>(v) / std::numeric_limits<int16_t>::max();
  }

  int16_t floatToSample(const float& v) {
    return std::numeric_limits<int16_t>::max() * v;
  }
}
