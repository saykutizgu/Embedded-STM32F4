#include "stm32f4xx.h"

void initHw();
void getTemp();
void calibration();

float rawTempVal;
float tempVal;

int main(void)
{
  initHw();

  while (1)
  {
	  getTemp();
	  calibration();
  }
}

void initHw(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);                                   // Enable ADC1 Clock

	ADC_InitTypeDef ADC_initStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;

	ADC_initStruct.ADC_ContinuousConvMode = ENABLE;                                        // ADC Configuration
	ADC_initStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	ADC_initStruct.ADC_NbrOfConversion = 1;
	ADC_initStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_initStruct.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1, &ADC_initStruct);
	ADC_CommonInit(&ADC_CommonInitStruct);
	ADC_CommonStructInit(&ADC_CommonInitStruct);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_144Cycles);   // ADC Channel Configuration

	ADC_TempSensorVrefintCmd(ENABLE);                                                      // Enable Temp Sensor and Vrefint Channel

	ADC_Cmd(ADC1, ENABLE);                                                                 // Enable ADC1
}

void getTemp(){

	ADC_SoftwareStartConv(ADC1);                                                           // Start Conversion

	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);                                 // Wait Until End of Communication Flag is Set

	rawTempVal = ADC_GetConversionValue(ADC1);                                             // Return the converted data

}

void calibration(){

	tempVal = ( ( ( rawTempVal * 3300 ) / ( ( 0xFFF ) * ( 1000.0 )  ) - ( 0.76 ) ) / ( 0.0025 ) ) + 25.0;

	/*
	TemperatureValue *= 3300;
	TemperatureValue /= 0xfff; //Reading in mV
	TemperatureValue /= 1000.0; //Reading in Volts
	TemperatureValue -= 0.760; // Subtract the reference voltage at 25°C
	TemperatureValue /= .0025; // Divide by slope 2.5mV
	TemperatureValue += 25.0; // Add the 25°C
	*/
							                                       // Calibration Values is Found Online for My Microcontroller's(STM32F401RE) Temperature Sensor
}
