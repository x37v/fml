//http://00xnor.blogspot.com/2014/01/6-stm32-f4-dac-dma-waveform-generator.html

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_dma.h"

//#include <math.h>
#include "arm_math.h"
#include "fmsynth.h"

#define   DAC_BUFFER_SIZE          256
#define   DAC_BUFFER_SIZE_2        128
#define   DAC_DHR12R1_ADDR  0x40007408                           // DMA writes into this reg on every request
#define   DAC_DHR12RD_Address 0x40007420

#define SINE_FREQ 420.0f
#define SR 44100.0f

//XXX approximately 44100, need to tune this value
static __IO uint16_t TIM6ARRValue = 1900;

#define TWO_PI 6.283185307179586
uint32_t dac_table[DAC_BUFFER_SIZE];

//we only compute 1/2 the buffer at a time so this is 1/2 of a buffer of stereo, interleaved
float synth_buffer[DAC_BUFFER_SIZE];

FMSynth * _synth;

/*
= { 2048, 2145, 2242, 2339, 2435, 2530, 2624, 2717, 2808, 2897,
                                      2984, 3069, 3151, 3230, 3307, 3381, 3451, 3518, 3581, 3640,
                                      3696, 3748, 3795, 3838, 3877, 3911, 3941, 3966, 3986, 4002,
                                      4013, 4019, 4020, 4016, 4008, 3995, 3977, 3954, 3926, 3894,
                                      3858, 3817, 3772, 3722, 3669, 3611, 3550, 3485, 3416, 3344,
                                      3269, 3191, 3110, 3027, 2941, 2853, 2763, 2671, 2578, 2483,
                                      2387, 2291, 2194, 2096, 1999, 1901, 1804, 1708, 1612, 1517,
                                      1424, 1332, 1242, 1154, 1068, 985, 904, 826, 751, 679,
                                      610, 545, 484, 426, 373, 323, 278, 237, 201, 169,
                                      141, 118, 100, 87, 79, 75, 76, 82, 93, 109,
                                      129, 154, 184, 218, 257, 300, 347, 399, 455, 514,
                                      577, 644, 714, 788, 865, 944, 1026, 1111, 1198, 1287,
                                      1378, 1471, 1565, 1660, 1756, 1853, 1950, 2047 };
                                      */

static void TIM6_Config(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

  TIM_DeInit(TIM6);
  TIM_SetAutoreload(TIM6, TIM6ARRValue);
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM6, ENABLE);
}

void NVIC_Configuration(void) {
  NVIC_InitTypeDef NVIC_InitStructure;
 
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void DAC1_Config(void)
{
  DAC_DeInit();

  DAC_InitTypeDef DAC_INIT;
  DMA_InitTypeDef DMA_INIT;

  DAC_StructInit(&DAC_INIT);

  DAC_INIT.DAC_Trigger        = DAC_Trigger_T6_TRGO;
  DAC_INIT.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_INIT.DAC_OutputBuffer   = DAC_OutputBuffer_Disable;

  //DAC_INIT.DAC_OutputBuffer   = DAC_OutputBuffer_Enable; //XXX caused some distortion
  DAC_Init(DAC_Channel_1, &DAC_INIT);
  DAC_Init(DAC_Channel_2, &DAC_INIT);

  DMA_DeInit(DMA1_Stream6);
  DMA_INIT.DMA_Channel            = DMA_Channel_7;
  DMA_INIT.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12RD_Address;
  DMA_INIT.DMA_Memory0BaseAddr    = (uint32_t)&dac_table;
  DMA_INIT.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
  DMA_INIT.DMA_BufferSize         = DAC_BUFFER_SIZE;
  DMA_INIT.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_INIT.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_INIT.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_INIT.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;
  DMA_INIT.DMA_Mode               = DMA_Mode_Circular;
  DMA_INIT.DMA_Priority           = DMA_Priority_High;
  DMA_INIT.DMA_FIFOMode           = DMA_FIFOMode_Disable;
  //DMA_INIT.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
  //DMA_INIT.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
  //DMA_INIT.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream6, &DMA_INIT);

  /* Enable DMA Stream Half Transfer and Transfer Complete interrupts */
  DMA_ITConfig(DMA1_Stream6, DMA_IT_HT | DMA_IT_TC, ENABLE);

  DMA_Cmd(DMA1_Stream6, ENABLE);

  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);

  DAC_DMACmd(DAC_Channel_2, ENABLE);
}

float sine_freq = SINE_FREQ;

inline uint32_t to_i(float v) {
  return static_cast<uint32_t>(2047.0f * (v + 1.0f));
}

void fill_buffer(uint32_t * mem, uint16_t size) {
#if 1
  _synth->compute(synth_buffer, size);

  for (uint16_t i = 0; i < size; i++) {
    uint16_t off = i << 1;
    mem[i] = to_i(synth_buffer[off]) << 16 | to_i(synth_buffer[off + 1]);
  }
#else
  static double index[2] = {0.0f, 0.0f};
  double inc = (440.0f / SR);

  for (int i = 0; i < size; i++) {
    uint32_t iv = to_i(compute_sine(index[0], 0));
    mem[i] = iv;

    iv = to_i(compute_sine(index[1], 0));
    mem[i] |= (iv << 16);

    index[0] += (sine_freq / SR);
    index[1] += inc;

    while (index[0] >= 1.0)
      index[0] -= 1.0;
    while (index[1] >= 1.0)
      index[1] -= 1.0;

    sine_freq += 0.01;
    if (sine_freq > 4000)
      sine_freq = 20.0f;
  }
#endif
}

volatile uint32_t * compute_buffer = 0;

extern "C"
void DMA1_Stream6_IRQHandler(void)
{
  /* Test on DMA Stream Half Transfer interrupt */
  if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_HTIF6)) {
    GPIO_SetBits(GPIOE, GPIO_Pin_12);
    GPIO_SetBits(GPIOE, GPIO_Pin_13);
    /* Add code to process First Half of Buffer */
    compute_buffer = dac_table;
    /* Clear DMA Stream Half Transfer interrupt pending bit */
    DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_HTIF6);
  }

  /* Test on DMA Stream Transfer Complete interrupt */
  if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6)) {
    GPIO_ResetBits(GPIOE, GPIO_Pin_12);
    GPIO_SetBits(GPIOE, GPIO_Pin_13);
    /* Add code to process Second Half of Buffer */
    compute_buffer = dac_table + DAC_BUFFER_SIZE_2;
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
  }

}

void dac_compute() {
  static int on = 0;
  if (compute_buffer) {
    uint32_t * buffer = (uint32_t *)compute_buffer;
    compute_buffer = 0;
    fill_buffer(buffer, DAC_BUFFER_SIZE_2);
    GPIO_ResetBits(GPIOE, GPIO_Pin_13);
  }
}

void dac_setup(FMSynth * synth) {
  _synth = synth;

  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  NVIC_Configuration();
  TIM6_Config();
  DAC1_Config();

  for (int i = 0; i < DAC_BUFFER_SIZE; i++) {
    double v = sin(((double)i / (DAC_BUFFER_SIZE - 1)) * TWO_PI);
    int32_t iv = 2048.0 * (v + 1.0);
    dac_table[i] = iv << 16;
  }
}

