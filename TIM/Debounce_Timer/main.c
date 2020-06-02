#include "stm32f4xx.h"

void INITIALIZE_HARDWARE(void);

uint8_t prevState = 0;                                                                                                         // Previous Button State
uint8_t buttonState = 0;                                                                                                       // Current Button State
uint8_t stSoftware;                                                                                                            // Current Software State

enum softwareState {IDLE, INTERRUPT_EXTI, INTERRUPT_TIM3};                                                                     // Software State Enumeration



int main(void)
{
	stSoftware = IDLE;                                                                                                         // Software State Set To IDLE by Default
	INITIALIZE_HARDWARE();                                                                                                     // Initializing Hardware Drivers
 while (1)
  {
	 prevState = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);                                                                    // Read Button State and Set to Previous
	 if(buttonState == 0){                                                                                                     // If Button Is Not Pressed
		 GPIO_ResetBits(GPIOA, GPIO_Pin_5);                                                                                    // Turn LED Off
	 }
	 else if(buttonState == 1){                                                                                                // If Button Is Pressed
		 GPIO_SetBits(GPIOA, GPIO_Pin_5);                                                                                      // Turn LED On

	 }
  }
}

void INITIALIZE_HARDWARE(void){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA , ENABLE);                                              // Initializing Clocks of Peripherals to be Used
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


	GPIO_InitTypeDef GPIO_initStruct;                                                                                          // Initializing Structs of Peripherals to be Used
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_initStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_initStruct;
	TIM_TimeBaseInitTypeDef TIM_timeBaseInitStruct;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                                                                               // LED Output Pin Configuration
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_initStruct.GPIO_Mode = GPIO_Mode_IN;                                                                                   // Button Input Pin Configuration
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOC, &GPIO_initStruct);

	TIM_timeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;                                                                    // Clock Division = 4  Which means my 24 MHz Clock is Divided by 4. New Clock Speed is 6 MHz
	TIM_timeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_timeBaseInitStruct.TIM_Period = 25;                                                                                     // An Update Flag will be Generated in Each 25 ms Cycles
	TIM_timeBaseInitStruct.TIM_Prescaler = 5999;                                                                                // 6 MHz / (Prescaler + 1) = New Clock Speed which is 1 kHz
	TIM_TimeBaseInit(TIM3, &TIM_timeBaseInitStruct);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                                                                                  // Update Interrupt Is Enabled

	NVIC_initStruct.NVIC_IRQChannel= EXTI15_10_IRQn;                                                                            // Because my Button Is Connected via PC_13 Pin, EXTI_15_10_IRQ Handler will be Used
	NVIC_initStruct.NVIC_IRQChannelCmd = ENABLE;																				// NVIC Enabled
	NVIC_initStruct.NVIC_IRQChannelPreemptionPriority = 2;                                                                      // Priority Selections (Timer Priority Should Be Higher In Order to Context Switch From EXTI Interrupt)
	NVIC_initStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_initStruct);

	NVIC_InitStructure.NVIC_IRQChannel= TIM3_IRQn;                                                                              // TIM3 will be Used to Generate a Debounce Timer and TIM3_IRQ Handler will be Used in NVIC
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;																				// NVIC Enabled
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;																	// Priority Selections (Timer Priority Should Be Higher In Order to Context Switch From EXTI Interrupt to TIM Interrupt)
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	EXTI_initStruct.EXTI_Line = EXTI_Line13;																					// Because my Button Is Connected via PC_13 Pin, EXTI_Line13 will be Used
	EXTI_initStruct.EXTI_LineCmd = ENABLE;																						// EXTI Line Enabled
	EXTI_initStruct.EXTI_Mode = EXTI_Mode_Interrupt;																			// Interrupt Mode
	EXTI_initStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;                                                                 // We want this interrupt to work both on Rising and Falling Edges of the Button Signal
	EXTI_Init(&EXTI_initStruct);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
}

void EXTI15_10_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line13)){                                                                                          // When EXTI Line is High
		if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) && prevState == 1){                                                       // Detect Rising Edge
			stSoftware = INTERRUPT_EXTI;                                                                                        // Software is now in EXTI_Interrupt
			TIM3->CNT = 0x00000000;                                                                                             // Reset Timer Counter Value
			TIM_Cmd(TIM3, ENABLE);                                                                                              // Enable Debounce Timer
		}
		else if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) && prevState == 0){                                                   // Detect Falling Edge
			TIM3->CNT = 0x00000000;                                                                                             // Reset Timer Counter Value
			TIM_Cmd(TIM3, ENABLE);                                                                                              // Enable Debounce Timer
		}
		EXTI_ClearITPendingBit(EXTI_Line13);                                                                                    // Exit Interrupt
		}
	stSoftware = IDLE;                                                                                                          // Software is now in IDLE
}

void TIM3_IRQHandler(void){
	if(TIM_GetITStatus(TIM3, TIM_IT_Update)){                                                                                   // When Timer Counter == Timer Period (25 ms Elapsed)
		stSoftware = INTERRUPT_TIM3;                                                                                            // Software is now in Timer_Interrupt
		buttonState = !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);                                                               // Read Current State of Button After 25 ms
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                                                                             // Exit Interrupt
	}
	TIM_Cmd(TIM3, DISABLE);                                                                                                     // Disable Timer
	stSoftware = IDLE;                                                                                                          // Software is now in IDLE
}
