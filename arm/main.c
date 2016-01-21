#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"

volatile uint32_t time_var1, time_var2;
// Private function prototypes
void Delay(volatile uint32_t nCount);
void init();

struct io_mapping {
  GPIO_TypeDef* port;
  uint16_t pin;
};

struct io_mapping buttons[3] = {
  {GPIOE, GPIO_Pin_8},
  {GPIOE, GPIO_Pin_9},
  {GPIOE, GPIO_Pin_11}
};

struct io_mapping leds[4] = {
  {GPIOB, GPIO_Pin_2},
  {GPIOE, GPIO_Pin_7},
  {GPIOE, GPIO_Pin_10},
  {GPIOE, GPIO_Pin_12}
};

int main(void) {
  init();

  for(;;) {
    for (int i = 0; i < 3; i++) {
      if (GPIO_ReadInputDataBit(buttons[i].port, buttons[i].pin)) {
        GPIO_SetBits(leds[i].port, leds[i].pin);
      } else {
        GPIO_ResetBits(leds[i].port, leds[i].pin);
      }
    }
    Delay(10);
  }

  return 0;
}

void setup_leds() {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  for (int i = 0; i < 4; i++) {
    GPIO_InitStructure.GPIO_Pin = leds[i].pin;
    GPIO_Init(leds[i].port, &GPIO_InitStructure);
    GPIO_ResetBits(leds[i].port, leds[i].pin);
  }
}

void setup_buttons() {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  for (int i = 0; i < 3; i++) {
    GPIO_InitStructure.GPIO_Pin = buttons[i].pin;
    GPIO_Init(buttons[i].port, &GPIO_InitStructure);
  }
}

void init() {

  // ---------- SysTick timer -------- //
  if (SysTick_Config(SystemCoreClock / 1000)) {
    // Capture error
    while (1){};
  }

  // ---------- GPIO -------- //
  // GIPO Periph clock enable
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  setup_leds();
  setup_buttons();
}

void SysTick_Handler(void)
{
  if (time_var1) {
    time_var1--;
  }

  time_var2++;
}

/*
 * Delay a number of systick cycles (1ms)
 */
void Delay(volatile uint32_t nCount) {
  time_var1 = nCount;
  while(time_var1){};
}

/*
 * Dummy function to avoid compiler error
 */
void _init() {

}

