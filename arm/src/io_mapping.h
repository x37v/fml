#ifndef IOMAPPING_H
#define IOMAPPING_H

#include "stm32f4xx.h"

struct io_mapping {
  GPIO_TypeDef* port;
  uint16_t pin;
};

#endif

