#include "util.h"
#include <cmath>

float fm::lin_smooth(float target, float current, float increment) {
  if (fabs(target - current) <= increment) {
    current = target;
  } else {
    current += (target > current) ? increment : -increment;
  }
  return current;
}
