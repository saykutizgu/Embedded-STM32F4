#include "stm32f4xx.h"
#include "stdio.h"
#include "stdlib.h"

void initHw();                                                                           // Function Prototyping

char name[9];
char *data;

int main(void){

	initHw();                                                                        // Initialize Hardware
	data = name;                                                                     // "Data" Pointer Points to "Name" Character Array

	while(1){

		sprintf(name, "Saykut\n");                                               // "Name" Character Array is Now "Saykut\n"
		USART_SendData(USART2, *data++);                                         // Send Data to Serial Monitor

	}
}

void initHw(){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);                            // USART2 GPIO Port Clock Enabled
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	 		         // USART2 Clock Enabled

	GPIO_InitTypeDef GPIO_initStruct;
	USART_InitTypeDef USART_initStruct;

	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF;                                        // USART GPIO Configuration
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIO_initStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);                        // GPIOA/Pin_2 is USART2's Alternate Function Pins

	USART_initStruct.USART_BaudRate = 9600;                                          // USART Configuration
	USART_initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_initStruct.USART_Mode = USART_Mode_Tx;                                     // Only Transmit Mode Enabled
	USART_initStruct.USART_Parity = USART_Parity_No;                                 // No Parity Mode
	USART_initStruct.USART_StopBits = USART_StopBits_1;                             
	USART_initStruct.USART_WordLength = USART_WordLength_8b;                         
	USART_Init(USART2, &USART_initStruct);
	USART_Cmd(USART2, ENABLE);

}
