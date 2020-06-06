#include "stm32f4xx.h"

uint32_t capture = 0;
uint32_t counter = 0;

static void INITIALIZE_DRIVERS(void);                                               // Function Prototyping

int main(void)
{
	INITIALIZE_DRIVERS();                                                           // Initialize Hardware Drivers

  while (1)
  {
		counter = TIM_GetCounter(TIM2);                                             // Read Timer 2 Counter In Each Step
  }
}

static void INITIALIZE_DRIVERS(void){

	GPIO_InitTypeDef GPIO_InitStruct;                                               // Peripheral Initialize Structures
	GPIO_InitTypeDef GPIO_initStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_TimeBaseInitTypeDef TIM_initStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);                            // Timer 2 Clock Enabled
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);                          // GPIOA Clock Enabled

	GPIO_initStructure.GPIO_Mode = GPIO_Mode_OUT;                                   // On-Board LED Pin GPIO Configuration
	GPIO_initStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_initStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIO_initStructure);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                                       // Input Capture Pin GPIO Configuration
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);                         // Configure Alternate Function Mode For This Pin

	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;                                    // Timer 2 Capture/Compare Interrupt Configuration Using NVIC
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);

	TIM_initStruct.TIM_ClockDivision = TIM_CKD_DIV4;                                // Timer 2 TimeBase Configuration
	TIM_initStruct.TIM_CounterMode = TIM_CounterMode_Up;                            // TIM_CKD_DIV4 Results in as a 6 MHz Clock
	TIM_initStruct.TIM_Period = 999;                                                // Period Value to Obtain 1 Second Period Update Event Interrupt
	TIM_initStruct.TIM_Prescaler = 5999;                                            // 6 MHz / (Prescaler + 1) = 1000 Hz
	                                                                                // I Selected 999 as Period Which Means 1000 / (Period + 1) = Clock Update Rate Which is 1 Second
	TIM_TimeBaseInit(TIM2, &TIM_initStruct);

	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;                                   // Timer 2 Channel 2 Input Capture Mode Configuration
	TIM_ICInitStruct.TIM_ICFilter = 0;                                              // No Filtering Needed on Input
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;                        // I want to Capture Rising Edge of the Input Signal
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;                              // No Prescaler Needed on Input
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;                    // DirectTI Mode Which Means Channel 1, 2, 3, 4 are Connected to CC1, CC2, CC3, CC4 Respectively

	TIM_ICInit(TIM2, &TIM_ICInitStruct);
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);                                         // Timer 2 Channel 2 Capture Compare Interrupt is Enabled

	TIM_Cmd(TIM2, ENABLE);                                                          // Start Timer 2

}

void TIM2_IRQHandler(void){

	if(TIM_GetITStatus(TIM2, TIM_IT_CC2)){                                          // When the Input Capture Pin of Timer 2 Detected a Rising Edge

		GPIO_ToggleBits(GPIOA, GPIO_Pin_5);                                         // Toggle On-Board LED
		capture = TIM_GetCapture2(TIM2);                                            // Measure Current Timer Counter

		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);                                    // Exit Interrupt
	}
}
