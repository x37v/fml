#include "adc.h"
#include "io_mapping.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx.h"
#include "fmsynth.h"
#include "midiproc.h"

/*
 * pin  adc in
PC0     10
PC1     11
PA0     0
PA1     1
PA2     2
PA3     3
PA6     6
PA7     7
PC4     14
PC5     15
PB0     8
PB1     9
*/

#define NUM_ADC 12

namespace {

  struct analog_input_mapping inputs[NUM_ADC] = {
    {GPIOC, GPIO_Pin_0, 10},
    {GPIOC, GPIO_Pin_1, 11},
    {GPIOA, GPIO_Pin_0, 0},
    {GPIOA, GPIO_Pin_1, 1},
    {GPIOA, GPIO_Pin_2, 2},
    {GPIOA, GPIO_Pin_3, 3},
    {GPIOA, GPIO_Pin_6, 6},
    {GPIOA, GPIO_Pin_7, 7},
    {GPIOC, GPIO_Pin_4, 14},
    {GPIOC, GPIO_Pin_5, 15},
    {GPIOB, GPIO_Pin_0, 8},
    {GPIOB, GPIO_Pin_1, 9},
  };

  uint8_t current_index = 0;
}


namespace adc {

  void start_conversion(uint8_t channel) {
    ADC_RegularChannelConfig(ADC1, inputs[channel].index, 1, ADC_SampleTime_3Cycles); //XXX arbitrary sample time setting
    // Start the conversion
    ADC_SoftwareStartConv(ADC1);
  }

  void init() {
    current_index = 0;

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

    start_conversion(current_index);
  }

#if 0
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
#endif

  void process(FMMidiProc& midiproc, FMSynth& synth) {
    //ditch if there isn't a conversion done
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
      return;

    //grab the value, start the next conversion
    uint16_t v = ADC_GetConversionValue(ADC1);

    const uint8_t index = current_index;
    current_index = (current_index + 1) % NUM_ADC;
    start_conversion(current_index);

    float fval = static_cast<float>(v) / 4096.0f;
    switch (index) {
      case 0:
        midiproc.process_float(synth, FMMidiProc::RATIO, fval);
        break;
      case 1:
        midiproc.process_float(synth, FMMidiProc::DEPTH, fval);
        break;
      case 2:
        midiproc.process_float(synth, FMMidiProc::MOD_ENV_ATK, fval);
        break;
      case 3:
        midiproc.process_float(synth, FMMidiProc::VOL_ENV_ATK, fval);
        break;
      case 4:
        midiproc.process_float(synth, FMMidiProc::FINE, fval);
        break;
      case 5:
        //midiproc.process_float(synth, FMMidiProc::VOLUME, fval);
        //synth.volume(fval);
        break;
      case 6:
        midiproc.process_float(synth, FMMidiProc::MOD_ENV_DEC, fval);
        break;
      case 7:
        midiproc.process_float(synth, FMMidiProc::VOL_ENV_REL, fval);
        break;
      case 8:
        midiproc.process_float(synth, FMMidiProc::FBDK, fval);
        break;
      case 9:
        break;
      case 10:
        break;
      case 11:
        break;
      default:
        break;
    }
  }
}
