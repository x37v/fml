#include "midi.h"
#include "stm32f4xx_conf.h"

#include <stm32f4xx.h>
#include <stm32f4xx_usart.h>


//http://www.mind-dump.net/stm32f4-discovery-midi-input-and-basic-synthesis

namespace midi {
  void init() {
    //https://github.com/g4lvanix/STM32F4-examples/blob/master/USART/main.c

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    //set up the peripheral
    USART_InitTypeDef usartis;
    USART_StructInit(&usartis);
    usartis.USART_BaudRate = 31250;
    usartis.USART_WordLength = USART_WordLength_8b;
    usartis.USART_StopBits = USART_StopBits_1;
    usartis.USART_Parity = USART_Parity_No;
    usartis.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usartis.USART_Mode = USART_Mode_Rx;
    USART_Init(USART3, &usartis);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    //set up the interrupt
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART3, ENABLE);
  }
}

extern "C"
void USART3_IRQHandler(void) {
  if(USART_GetITStatus(USART3, USART_FLAG_RXNE)) {
    USART_ClearITPendingBit(USART3, USART_FLAG_RXNE);
  }
}
