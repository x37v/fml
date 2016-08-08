#ifndef IOMAPPING_H
#define IOMAPPING_H

#include "stm32f4xx.h"

struct io_mapping {
  GPIO_TypeDef* port;
  uint16_t pin;
};

struct analog_input_mapping {
  GPIO_TypeDef* port;
  uint16_t pin;
  uint8_t index;
};

#endif

