#include "defines.h"
#include <arm_math.h>

namespace fm {
  float time_to_increment(float v) {
    if (v < 0.015)
      v = 0.015;
    return fm::fsample_period() / v;
  }
}

