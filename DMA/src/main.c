#include "stm32f4xx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void initHw();

const char output[] = "Saykut Says Hello.";                                          // Buffer to Transmit Through USART If Button is Pressed                 
const char noButton[] = "Push the Button.";                                          // Buffer to Transmit If Button is not Pressed

uint8_t length = strlen(output);                                                     // String Length of "output" Buffer
uint8_t noButLen = strlen(noButton);                                                 // String Length of "noButton" Buffer

char errorMessage[] = "Transfer Error";                                              // Buffer to Transmit If Interrupt is Called

char *message;                                                                       // Interrupt Message Buffer Pointer

int main(void){


	initHw();                                                                    // Initialize Hardware
	message = errorMessage;                                                      // Pointer Belongs to Buffer

	while(1){                                                                    // Infinite Loop

		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1){                  // If USER Button Pressed
			for(int i=0; i<length;i++){                                  // For with Length of Buffer
				USART2->DR = output[i];                              // Send Output Buffer to USART Data Register via DMA
				                  }
		      }
		else                                                                 // If USER Button is not Pressed
		      {
				for(int a=0; a<noButLen;a++){                        // For with "noButLen" of Buffer
				 USART2->DR = noButton[a];                           // Send No Button Buffer to USART via DMA
				                          }
		      }
		}

}

void initHw(){                                                                       // Hardware Initializing

	RCC->CFGR = 0;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);      		     // USART2 GPIO Port
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_GPIOC, ENABLE);  // USER Button & DMA Clock Enabled
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	 		     // USART2 Clock Enabled
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);			     // SYSCFG Clock Enabled

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitTypeDef GPIO_initStruct;
	USART_InitTypeDef USART_initStruct;
	DMA_InitTypeDef DMA_initStruct;
	NVIC_InitTypeDef NVIC_initStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;                                    // USER_BUTTON Configuration
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                                  // Push-Pull
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;                                  // Pull Down
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF;                                    // USART GPIO Configuration
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;                                  // Push-Pull
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_2;                                    
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_UP;                                    // Pull Up
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIO_initStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);                    // GPIOA/Pin_2 is USART2's Alternate Function Pins

	DMA_initStruct.DMA_Channel = DMA_Channel_4;                                  // DMA Channel 4 Selected (USART2 Uses DMA1 Channel 4, Stream 6)
	DMA_initStruct.DMA_PeripheralBaseAddr = (uint32_t) &(USART2->DR);            // Memory Address of Destination
	DMA_initStruct.DMA_Memory0BaseAddr = (uint32_t) (output);		     // Memory Address of Start
	DMA_initStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;                         // Memory to Peripheral Direction
	DMA_initStruct.DMA_BufferSize = length;                                      // Length of Buffer
	DMA_initStruct.DMA_MemoryDataSize = 8;                                       // 8 bits
	DMA_initStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;                         // Memory Increment Enabled (output[1] to output[2])
	DMA_initStruct.DMA_Mode = DMA_Mode_Circular;                                 // Circular Mode (start again after transmission complete)
	DMA_initStruct.DMA_Priority = DMA_Priority_High;                             // High Priority
	DMA_initStruct.DMA_PeripheralDataSize = 8;                                   // 8 bits
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TEIF6, ENABLE);                            // DMA Transfer Error Interrupt Enabled
	DMA_Init(DMA1_Stream6, &DMA_initStruct);                                     
	DMA_Cmd(DMA1_Stream6, ENABLE);                                               // DMA1 Stream 6 Channel 4 Enabled

	USART_initStruct.USART_BaudRate = 9600;                                      // USART Configuration
	USART_initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_initStruct.USART_Mode = USART_Mode_Tx;                                 // Only Transmit Mode Enabled
	USART_initStruct.USART_Parity = USART_Parity_No;                             // No Parity Mode
	USART_initStruct.USART_StopBits = USART_StopBits_1;
	USART_initStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_initStruct);
	USART_Cmd(USART2, ENABLE);

	NVIC_initStruct.NVIC_IRQChannel = DMA1_Stream6_IRQn;			     // DMA Nested Vector Interrupt Controller
	NVIC_initStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_initStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_initStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_initStruct);
}

void DMA1_Stream6_IRQ(void){                                                         // DMA1 Stream6 Interrupt Request Function
	while(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TEIF6) != RESET ){                // If Transfer Results with an Error
		USART_SendData(USART2, *message++);                                  // Send Error Message
		DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TEIF6);                   // Clear Interrupt Bit
	}
}
