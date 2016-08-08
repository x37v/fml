#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "dac.h"
#include "fmsynth.h"
#include "midi.h"
#include "io_mapping.h"
#include "adc.h"

/*
 *
 *   POTS  LED  BUTTONS  DAC  MIDI                DIP switch
 *   PC0   PB2  PE8      PA4  PB11 (Rx USART3)    PD8
 *   PC1   PE7  PE9      PA5                      PD9
 *   PA0   PE10 PE11                              PD10
 *   PA1   PE12                                   PD11
 *   PA2
 *   PA3
 *   PA6
 *   PA7
 *   PC4
 *   PC5
 *   PB0
 *   PB1
 *
 */

volatile uint32_t time_var1, time_var2;
// Private function prototypes
void Delay(volatile uint32_t nCount);
void init(FMSynth * synth);

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
  FMSynth synth;
  uint8_t button_down = 0;

  init(&synth);

  synth.volume(0.1);
  synth.mod_depth(0.9);
  synth.mode(FMVoice::NORMAL);
  synth.volume_envelope_setting(ADSR::env_release, 2.0);
  synth.mod_envelope_setting(ADSR::env_attack, 4.0);
  synth.mod_envelope_setting(ADSR::env_release, 4.0);
  synth.freq_mult(1.0, 4.0);
  synth.modulator_freq_offset(0.5);
  synth.feedback(0.2);
  //synth.trigger(0, true, 50, 0.5);
  //synth.trigger(1, true, 73, 0.5);

  //synth.trigger(2, true, 72, 0.5);
  //synth.trigger(4, true, 80, 0.5);
  //synth.trigger(3, true, 3, 0.5);
  //synth.trigger(5, true, 1, 0.5);
  //
  GPIO_SetBits(leds[0].port, leds[0].pin);
  GPIO_SetBits(leds[1].port, leds[1].pin);
  GPIO_SetBits(leds[2].port, leds[2].pin);
  GPIO_SetBits(leds[3].port, leds[3].pin);
  //GPIO_ResetBits(leds[1].port, leds[1].pin);

  for(;;) {
    dac_compute();
    adc::process(&synth);
    for (uint8_t i = 0; i < 3; i++) {
      uint8_t val = GPIO_ReadInputDataBit(buttons[i].port, buttons[i].pin);
      uint8_t mask = (1 << i);
      uint8_t led_index = i;
      if (i == 1)
        led_index = 3;
      if (val) {
        if (!(button_down & mask)) {
          synth.trigger(i, false, 50 + i * 4, 0.5);
          //synth.process_note(false, 0, 64 + i * 10, 127);
          button_down |= mask;
          GPIO_SetBits(leds[led_index].port, leds[led_index].pin);
        }
      } else {
        if (button_down & mask) {
          synth.trigger(i, true, 50 + i * 4, 1.0);
          //synth.process_note(false, 0, 64 + i * 10, 127);
          //synth.process_note(true, 0, 64 + i * 10, 127);
          button_down &= ~mask;
          GPIO_ResetBits(leds[led_index].port, leds[led_index].pin);
        }
      }
    }
    //Delay(10);
  }

  return 0;
}

void leds_setup() {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  for (auto led: leds) {
    GPIO_InitStructure.GPIO_Pin = led.pin;
    GPIO_Init(led.port, &GPIO_InitStructure);
    GPIO_ResetBits(led.port, led.pin);
  }
}

void buttons_setup() {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  for (auto button: buttons) {
    GPIO_InitStructure.GPIO_Pin = button.pin;
    GPIO_Init(button.port, &GPIO_InitStructure);
  }
}

void init(FMSynth * synth) {
  /*
  // ---------- SysTick timer -------- //
  if (SysTick_Config(SystemCoreClock / 1000)) {
    // Capture error
    while (1){};
  }
  */

  // ---------- GPIO -------- //
  // GIPO Periph clock enable
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  leds_setup();
  buttons_setup();
  dac_setup(synth);
  midi::init();
  adc::init();
}

extern "C"
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
extern "C"
void _init() {

}
