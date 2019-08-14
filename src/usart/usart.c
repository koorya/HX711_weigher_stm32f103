#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>


#define BUFF_SIZE 256

char send_buffer[BUFF_SIZE];

void initUsart(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	USART_Cmd(USART1, ENABLE);
	USART_SendData(USART1, 0xfa);


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (int)&(USART1->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (int)send_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
}

uint8_t sendStringByUSART(char * str){
	if (DMA_GetCurrDataCounter(DMA1_Channel4))
		return 1;
	sprintf(send_buffer, "%s", str);
	DMA_Cmd(DMA1_Channel4, DISABLE);
	uint16_t str_length;
	str_length = strlen(send_buffer);
	DMA_SetCurrDataCounter(DMA1_Channel4, str_length);
	DMA_Cmd(DMA1_Channel4, ENABLE);
	return 0;
}



