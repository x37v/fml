#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"

volatile uint32_t time_var1, time_var2;
// Private function prototypes
void Delay(volatile uint32_t nCount);
void init();

int main(void) {
  init();

  for(;;) {
    GPIO_SetBits(GPIOB, GPIO_Pin_2);
    GPIO_SetBits(GPIOE, GPIO_Pin_7);
    GPIO_SetBits(GPIOE, GPIO_Pin_10);
    GPIO_SetBits(GPIOE, GPIO_Pin_12);
    Delay(500);
    GPIO_ResetBits(GPIOB, GPIO_Pin_2);
    GPIO_ResetBits(GPIOE, GPIO_Pin_7);
    GPIO_ResetBits(GPIOE, GPIO_Pin_10);
    GPIO_ResetBits(GPIOE, GPIO_Pin_12);
    Delay(500);
  }

  return 0;
}

void init() {
  GPIO_InitTypeDef  GPIO_InitStructure;

  // ---------- SysTick timer -------- //
  if (SysTick_Config(SystemCoreClock / 1000)) {
    // Capture error
    while (1){};
  }

  // ---------- GPIO -------- //
  // GIPO Periph clock enable
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_12;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_SetBits(GPIOB, GPIO_Pin_2);
  GPIO_SetBits(GPIOE, GPIO_Pin_7);
  GPIO_SetBits(GPIOE, GPIO_Pin_10);
  GPIO_SetBits(GPIOE, GPIO_Pin_12);
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

