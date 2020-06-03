#include "stm32f4xx.h"

void INITIALIZE_HARDWARE_DRIVERS(void);                                                                          // Function Prototyping

uint8_t cntCC1 = 0;
uint8_t cntCC2 = 0;
uint8_t cntCC3 = 0;

TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;                                                                  // Peripheral Structs Defined Here to Use in Interrupt
NVIC_InitTypeDef NVIC_initStruct;
TIM_OCInitTypeDef TIM_OCInitStruct;
GPIO_InitTypeDef GPIO_initStruct;
TIM_ICInitTypeDef TIM_ICInitStruct;

int main(void)
{
	INITIALIZE_HARDWARE_DRIVERS();                                                                               // Initialize Drivers
 while (1)
  {
  }
}

void INITIALIZE_HARDWARE_DRIVERS(void){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);          // Enable GPIO Clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM2, ENABLE);                                   // Enable Timer Clocks

	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF;                                                                    // Timer 2 Input Capture Pin GPIO Settings
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOA, &GPIO_initStruct);

	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF;                                                                    // Timer 3 Channel 1 & 2 Output Compare Pin GPIO Settings
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOB, &GPIO_initStruct);

	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF;                                                                    // Timer 3 Channel 3 Output Compare Pin GPIO Settings
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOC, &GPIO_initStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4 | GPIO_PinSource5, GPIO_AF_TIM3);                                    // Pin AF Configurations According to Datasheet Alternate Function Map
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);


	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;                                                     // Timer 3 Time Base Settings
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 1999;                                                                    // Period is doubled in Output Compare Because Timer Works x2 Times Faster
	TIM_TimeBaseInitStruct.TIM_Prescaler = 5999;                                                                 // This Prescaler and Period Makes Timer Work in 1 Hz.
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;                                                    // Timer 3 Channel 1 Output Compare Settings
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;                                                             // Toggle Mode (When Pulse == Tim_Counter Toggle Output)
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 500;

	TIM_OC1Init(TIM3, &TIM_OCInitStruct);

	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;                                                               // Timer 3 Channel 2 PWM Mode (Pulse / Timer_Period == Duty Cycle)
	TIM_OCInitStruct.TIM_Pulse = 1000;

	TIM_OC2Init(TIM3, &TIM_OCInitStruct);

	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Active;                                                             // Timer 3 Channel 3 Active Mode (Pulse == Tim_Counter Sets Output High)
	TIM_OCInitStruct.TIM_Pulse = 250;

	TIM_OC3Init(TIM3, &TIM_OCInitStruct);

	TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3, ENABLE);                                            // Enable Channel 1, 2 and 3 for Capture/Compare Interrupt

	TIM_Cmd(TIM3, ENABLE);                                                                                       // Enable Timer 3

	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;                                                     // Timer 2 Time Base Settings(My Clock is 24 MHz)
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 999;                                                                     // Period is calculated to obtain 1 second update rate from timer.
	TIM_TimeBaseInitStruct.TIM_Prescaler = 5999;                                                                 // Prescaler value for 1 Hz.
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;                                                                // Input Capture Settings For Timer 2 Channel 2
	TIM_ICInitStruct.TIM_ICFilter = 0;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;                                                    // Capture at Falling Edge of Input Signal
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;

	TIM_ICInit(TIM2, &TIM_ICInitStruct);

	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);                                                                      // Enable Timer 2 Channel 2 for Capture/Compare Interrupt

	TIM_Cmd(TIM2, ENABLE);                                                                                       // Enable Timer 2

	NVIC_initStruct.NVIC_IRQChannel = TIM3_IRQn;                                                                 // Timer 3 Interrupt Handler is fed to NVIC
	NVIC_initStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_initStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_initStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_initStruct);


	NVIC_initStruct.NVIC_IRQChannel = TIM2_IRQn;                                                                 // Timer 2 Interrupt Handler is fed to NVIC
	NVIC_initStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_initStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_initStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_initStruct);

}

void TIM3_IRQHandler(void){
	if(TIM_GetITStatus(TIM3, TIM_IT_CC1)){                                                                       // Timer 3 Channel 1 Interrupt (Output Toggled When Counter == Pulse)
		cntCC1 = TIM_GetCounter(TIM3);																			 // Measuring Timer Counter when Interrupt happens
 		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);                                                                 // Exit Interrupt
	}

	if(TIM_GetITStatus(TIM3, TIM_IT_CC2)){                                                                       // Timer 3 Channel 2 Interrupt (Output High When Counter < Pulse)
		cntCC2 = TIM_GetCounter(TIM3);                                                                           // Measuring Timer Counter when Interrupt happens
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);                                                                 // Exit Interrupt
	}

	if(TIM_GetITStatus(TIM3, TIM_IT_CC3)){                                                                       // Timer 3 Channel 3 Interrupt (Output High When Counter > Pulse (Active Mode))
		TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Inactive;                                                       // Set New Pulse Value to Obtain A Square Wave (Output Low When Counter > Pulse (Inactive Mode))
		TIM_OCInitStruct.TIM_Pulse = 1250;

		TIM_OC3Init(TIM3, &TIM_OCInitStruct);
		cntCC3 = TIM_GetCounter(TIM3);                                                                           // Measuring Timer Counter when Interrupt happens
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);                                                                 // Exit Interrupt
	}

}

void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2)){                                                                       // Timer 3 Channel 3 Output Is Connected to Timer 2 Channel 2 Input Capture
                                                                                                                 // When The Output of Tim3Ch3 Is In Falling Edge (Tim2Ch2 is Configured As Falling Edge Input Capture)
		TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Active;                                                         // Timer 2 Interrupt Happens, Sets a New Active Mode for Tim3Ch3 Output Compare
		TIM_OCInitStruct.TIM_Pulse = 250;                                                                        // So At First, Tim3Ch3 Starts Low, When Tim3Ch3 Counter == Pulse (250), Output Goes High
                                                                                                                 // After that Tim3Ch3 Interrupt Happens, Sets New Pulse Value, When Tim3Ch3 Counter == Pulse (1250), Output Goes Low
		TIM_OC3Init(TIM3, &TIM_OCInitStruct);                                                                    // When Output of Tim3Ch3 Goes Low, Tim2Ch2 Input Capture Interrupt Happens
		                                                                                                         // Tim2Ch2 Interrupt Sets New Pulse Value, When Tim3Ch3 Counter == Pulse (250), Output Goes High
																												 // This Cycle Goes On and On and We have A Square Wave In the Tim3Ch3 Output
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
		}
}
