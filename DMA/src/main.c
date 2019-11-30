#include "stm32f4xx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void initHw();

char name[6];

char *data;

uint32_t startAddress = (uint32_t) &name[0];

uint32_t length = sizeof(name);

int main(void){

	data = name;                                                                     // Pointer of Name

	sprintf(name, "Saykut\n");                                                       // Name is Saykut

	initHw();                                                                        // Initialize Hardware

	uint8_t swtEnBitVal = DMA_GetCmdStatus(DMA1_Stream6);                            // Enable Bit Check

	uint8_t numRemainingData = DMA_GetCurrDataCounter(DMA1_Stream6);                 // Data Counter Check

	while(1){

		uint8_t a = swtEnBitVal;                                                     // Check if Enable Bit is Set
		uint8_t b = numRemainingData;												 // Check the Number of Remaining Data in DMA Buffer

	}
}

void initHw(){                                                                       // Hardware Initializing

	uint8_t clockSource = RCC_GetSYSCLKSource();                                     // Check Clock Source
	RCC->CFGR = 0;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_DMA1, ENABLE);      // USART2 GPIO Port & DMA Clock Enabled
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	 		                 // USART2 Clock Enabled

	GPIO_InitTypeDef GPIO_initStruct;
	USART_InitTypeDef USART_initStruct;
	DMA_InitTypeDef DMA_initStruct;

	DMA_initStruct.DMA_Channel = DMA_Channel_4;                                      // DMA Configuration
	DMA_initStruct.DMA_PeripheralBaseAddr = (uint32_t) &(USART2->DR);                // Sent Data to USART2 Data Register Address
	DMA_initStruct.DMA_Memory0BaseAddr = startAddress;                               // Get Data from Name[0] Address
	DMA_initStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;                             // Memory To Peripheral Flow
	DMA_initStruct.DMA_BufferSize = length;                                          // Size of Data to be Sent
	DMA_initStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;                             // Increment Memory Address by 1 After Sent (Gets to the Next Address to Sent Data from)
	DMA_initStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;                     // Increment Peripheral Address by 1 After Sent (Gets to the Next Address to Sent Data to)
	DMA_initStruct.DMA_Mode = DMA_Mode_Circular;                                     // Circular Mode Active
	DMA_Init(DMA1_Stream6, &DMA_initStruct);
	DMA_Cmd(DMA1_Stream6, ENABLE);                                                   // Enable DMA Peripheral

	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF;                                        // USART GPIO Configuration
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;                                      // GPIO Type Push-Pull
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;                              // GPIO Pins to Use
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_UP;                                        // GPIO Pull Up Mode
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_25MHz;                                   // GPIO Speed
	GPIO_Init(GPIOA, &GPIO_initStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2 | GPIO_PinSource3, GPIO_AF_USART2);      // GPIOA/Pin_2 is USART2's Alternate Function Pins

	USART_initStruct.USART_BaudRate = 115200;                                        // USART Configuration
	USART_initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     // No Flow Control
	USART_initStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                     // Transmit and Receive Mode Active
	USART_initStruct.USART_Parity = USART_Parity_No;                                 // No Parity Bits
	USART_initStruct.USART_StopBits = USART_StopBits_1;                              // 1 Stop Bit
	USART_initStruct.USART_WordLength = USART_WordLength_8b;                         // Word Length is 8 Bits
	USART_Init(USART2, &USART_initStruct);
	USART_Cmd(USART2, ENABLE);                                                       // Enable USART Peripheral
	USART_DMACmd(USART2, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);                 // Enable USART's DMA Command

}
