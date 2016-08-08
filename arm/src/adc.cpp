#include "adc.h"
#include "io_mapping.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx.h"
#include "fmsynth.h"

/*
PC0  ADC123_IN10
PC1
PA0
PA1
PA2
PA3
PA6
PA7
PC4
PC5
PB0
PB1
*/

#define NUM_ADC 12

namespace {

  struct io_mapping inputs[NUM_ADC] = {
    {GPIOC, GPIO_Pin_0},
    {GPIOC, GPIO_Pin_1},
    {GPIOA, GPIO_Pin_0},
    {GPIOA, GPIO_Pin_1},
    {GPIOA, GPIO_Pin_2},
    {GPIOA, GPIO_Pin_3},
    {GPIOA, GPIO_Pin_6},
    {GPIOA, GPIO_Pin_7},
    {GPIOC, GPIO_Pin_4},
    {GPIOC, GPIO_Pin_5},
    {GPIOB, GPIO_Pin_0},
    {GPIOB, GPIO_Pin_1},
  };
}


namespace adc {
  void init() {

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    //single conversion mode:
    //http://www.micromouseonline.com/2009/05/26/simple-adc-use-on-the-stm32/
    //
    /* Put everything back to power-on defaults */
    ADC_DeInit();
    
    ADC_CommonInitTypeDef ADC_Common;
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_CommonStructInit(&ADC_Common);
    ADC_StructInit(&ADC_InitStructure);
    GPIO_StructInit(&GPIO_InitStructure);


    for (int i = 0; i < NUM_ADC; i++) {
      GPIO_InitStructure.GPIO_Pin = inputs[i].pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(inputs[i].port, &GPIO_InitStructure);
    }

    ADC_Common.ADC_Mode = ADC_Mode_Independent;
    ADC_Common.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;

    //XXX arbitrary
    ADC_Common.ADC_Prescaler = ADC_Prescaler_Div8;
    ADC_Common.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;


    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;

    ADC_InitStructure.ADC_ScanConvMode = DISABLE; /* Disable the scan conversion so we do one at a time */
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; /* Don't do contimuous conversions - do them on demand */
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; /* Start conversin by software, not an external trigger */
    ADC_InitStructure.ADC_ExternalTrigConv = 0; //XXX not used?
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; /* Conversions are 12 bit - put them in the lower 12 bits of the result */
    ADC_InitStructure.ADC_NbrOfConversion = 1; /* Say how many channels would be used by the sequencer */

    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_Cmd(ADC1, ENABLE);

#if 0
    /* Enable ADC1 reset calibaration register */
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));
    /* Start ADC1 calibaration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));
#endif
  }

  uint16_t readADC1(u8 channel)
  {
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_3Cycles); //XXX arbitrary sample time setting
    // Start the conversion
    ADC_SoftwareStartConv(ADC1);
    // Wait until conversion completion
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    // Get the conversion value
    return ADC_GetConversionValue(ADC1);
  }

  void process(FMSynth * synth) {
    uint16_t v = readADC1(10); //XXX c0
    if (v > 255) {
      synth->feedback(0.5);
    }
  }

}
