#include "stm32f4xx.h"

void initHw();

int main(void){

initHw();

  while (1){

  /* Doing everything in the IRQs */

  }
}

void initHw(){

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			   /* Timer, GPIO and SYSCFG(For External Interrupt) Clocks Enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitTypeDef GPIO_initStruct;
	GPIO_InitTypeDef GPIO_initStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_initStruct;
	NVIC_InitTypeDef NVIC_initStructure;
	EXTI_InitTypeDef EXTI_initStruct;

	GPIO_initStruct.GPIO_Mode = GPIO_Mode_IN;                                  /* USER Button GPIO Configuration */
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_initStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;                                 /* External LED GPIO Configuration */
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_initStructure.GPIO_Mode = GPIO_Mode_OUT;                              /* USER LED GPIO Configuration */
	GPIO_initStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_initStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_initStructure);

	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 9999;				   /* Counts up with 1 Second Period */
	TIM_TimeBaseInitStruct.TIM_Prescaler = 8399;                               /* Timer frequency = (Timer Default Frequency) / (prescaler + 1)  In our case Timer_Frequency = 84000000 / 8400 = 10 kHz */
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 50;                         /* Count Repetitive */
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	NVIC_initStruct.NVIC_IRQChannel = TIM3_IRQn;                               /* Internal Interrupt with Timer 3 Configuration */
	NVIC_initStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_initStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_initStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_initStruct);

	EXTI_initStruct.EXTI_Line = EXTI_Line13;                                   /* External Interrupt Line Configuration */
	EXTI_initStruct.EXTI_LineCmd = ENABLE;
	EXTI_initStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_initStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_initStruct);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

	NVIC_initStructure.NVIC_IRQChannel = EXTI15_10_IRQn;                       /* External Interrupt NVIC Configuration */
	NVIC_initStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_initStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_initStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_initStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_5);					   /* Setting the interrupted pin to 1 initially.*/

}

void EXTI15_10_IRQHandler(void) {
		if (EXTI_GetITStatus(EXTI_Line13) != RESET) {                      /* When External Interrupt Line is not 0 */
			GPIO_ToggleBits(GPIOA, GPIO_Pin_8);                        /* Toggle The External LED */
			EXTI_ClearITPendingBit(EXTI_Line13);                       /* Clear External Interrupt Line Bit */
		}
	}

void TIM3_IRQHandler()
	{
		if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET){                  /* When Timer 3 is Set */
			GPIO_ToggleBits(GPIOA, GPIO_Pin_5);                        /* Toggle USER_LED */
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                /* Clear Timer Bit */
			}
	}
