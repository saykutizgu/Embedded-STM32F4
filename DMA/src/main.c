#include "stm32f4xx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void initHw();

const char output[] = "Saykut Says Hello.";
const char noButton[] = "Push the Button.";

uint8_t length = strlen(output);
uint8_t noButLen = strlen(noButton);

char errorMessage[] = "Transfer Error";

char *message;

int main(void){


	initHw();
	message = errorMessage;

	while(1){

		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1){
			for(int i=0; i<length;i++){
				USART2->DR = output[i];
				}
		      }
		else
		      {
				for(int a=0; a<length;a++){
				 USART2->DR = noButton[a];
				 }
			  }
		    }

}

void initHw(){                                                                       // Hardware Initializing

	RCC_GetSYSCLKSource();
	RCC->CFGR = 0;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_GPIOC, ENABLE);      // USART2 GPIO Port & DMA Clock Enabled
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	 		                 // USART2 Clock Enabled
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitTypeDef GPIO_initStruct;
	USART_InitTypeDef USART_initStruct;
	DMA_InitTypeDef DMA_initStruct;
	NVIC_InitTypeDef NVIC_initStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;                                        // USER_BUTTON Configuration
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF;                                        // USART GPIO Configuration
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIO_initStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);                        // GPIOA/Pin_2 is USART2's Alternate Function Pins

	DMA_initStruct.DMA_Channel = DMA_Channel_4;                                      // DMA Configuration
	DMA_initStruct.DMA_PeripheralBaseAddr = (uint32_t) &(USART2->DR);
	DMA_initStruct.DMA_Memory0BaseAddr = (uint32_t) (output);
	DMA_initStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_initStruct.DMA_BufferSize = length;
	DMA_initStruct.DMA_MemoryDataSize = 8;
	DMA_initStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_initStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_initStruct.DMA_Priority = DMA_Priority_High;
	DMA_initStruct.DMA_PeripheralDataSize = 8;
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TEIF6, ENABLE);
	DMA_Init(DMA1_Stream6, &DMA_initStruct);
	DMA_Cmd(DMA1_Stream6, ENABLE);

	USART_initStruct.USART_BaudRate = 9600;                                          // USART Configuration
	USART_initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_initStruct.USART_Mode = USART_Mode_Tx;                                     // Only Transmit Mode Enabled
	USART_initStruct.USART_Parity = USART_Parity_No;                                 // No Parity Mode
	USART_initStruct.USART_StopBits = USART_StopBits_1;
	USART_initStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_initStruct);
	USART_Cmd(USART2, ENABLE);

	NVIC_initStruct.NVIC_IRQChannel = DMA1_Stream6_IRQn;							 // DMA Nested Vector Interrupt Controller
	NVIC_initStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_initStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_initStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_initStruct);
}

void DMA1_Stream6_IRQ(void){
	while(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TEIF6) != RESET ){
		USART_SendData(USART2, *message++);
		DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TEIF6);
	}
}
