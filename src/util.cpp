#include "util.h"
#include <cmath>

float fm::lin_smooth(float target, float current, float increment) {
#if 0
  return target;
#else
  //XXX shouldn't happen
  if (increment == 0)
    return target;

  if (increment > 0) {
    if (target <= current)
      return target;
  } else {
    if (target >= current)
      return target;
  }
  return current + increment;
#endif
}

