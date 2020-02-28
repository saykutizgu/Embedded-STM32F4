#include "stm32f4xx.h"

uint32_t hours, minutes, seconds, weekday, date, month, year;

uint8_t whichDay(uint8_t weekDay);

void INIT_RTC_STD_PERIPH_DRIVER_FUNC();

enum WeekDays {
			UNKNOWN, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY
} day;

RTC_InitTypeDef RTC_initStruct;
RTC_TimeTypeDef RTC_timeInitStruct;
RTC_DateTypeDef RTC_dateInitStruct;
RTC_AlarmTypeDef RTC_AlarmStruct;

int main(void)
{

	INIT_RTC_STD_PERIPH_DRIVER_FUNC();                                                                // Initialize RTC Driver

	enum WeekDays thisDay;                                                                            // "thisDay" is an Enumerated variable

  while (1)
  {
	  hours   = (((RTC->TR) & 0xF0000)  >> 16)      + (((RTC->TR) & 0x300000)  >> 20) * 16;           // Read Hours
	  minutes = (((RTC->TR) & 0xF00)    >> 8 )      + (((RTC->TR) & 0x7000) >> 12) * 16;              // Read Minutes
	  seconds = (((RTC->TR) & 0x70)     >> 4 ) * 10 + ((RTC->TR) & 0x0F);                             // Read Seconds
	  weekday = (((RTC->DR) & 0xE000)  >> 13);                                                        // Read Weekday Number
	  thisDay = whichDay(weekday);                                                                    // Weekday Number to Enum
	  date    = (RTC->DR) & 0xFF;                                                                     // Read Date
          month   = (((RTC->DR) & 0x1000)   >> 12) * 10 + (((RTC->DR) & 0xF00) >> 8);                     // Read Month
          year    = (((RTC->DR) & 0xFF0000) >> 16)      + 2000;                                           // Read Year
  }
}

void INIT_RTC_STD_PERIPH_DRIVER_FUNC(){

	RCC_BackupResetCmd(DISABLE);                                                                      // Disable Reset Backup Registers Command
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);                                               // Enable  PWR Peripheral Clock
	PWR_BackupAccessCmd(ENABLE);                                                                      // Enable  Backup Register Access
	RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div25);                                                     // Source  Clock Config
	RCC_RTCCLKCmd(ENABLE);                                                                            // Enable  RTC Clock


/******************************************GENERAL SETTINGS*********************************************/


	RTC_initStruct.RTC_AsynchPrediv = 124;                                                            // Asynchronous Predivision
	RTC_initStruct.RTC_SynchPrediv = 7999;                                                            // Synchronous  Predivision
	RTC_initStruct.RTC_HourFormat = RTC_HourFormat_24;                                                // Hour Format
	RTC_Init(&RTC_initStruct);                                                                        // Initialize General Settings


/****************************************TIME CONFIGURATION*********************************************/


	RTC_timeInitStruct.RTC_H12 = RTC_H12_PM;                                                          // AM-PM Config
	RTC_timeInitStruct.RTC_Hours = 20;                                                                // Hours to Start
	RTC_timeInitStruct.RTC_Minutes = 05;                                                              // Minutes to Start
	RTC_timeInitStruct.RTC_Seconds = 00;                                                              // Seconds to Start

	RTC_SetTime(RTC_Format_BCD, &RTC_timeInitStruct);                                                 // Set Time in BCD Format
	
/****************************************DATE CONFIGURATION*********************************************/


	RTC_dateInitStruct.RTC_Date = 30;                                                                 // Date to Start
	RTC_dateInitStruct.RTC_Month = RTC_Month_November;                                                // Month to Start
	RTC_dateInitStruct.RTC_WeekDay = RTC_Weekday_Saturday;                                            // Weekday to Start
	RTC_dateInitStruct.RTC_Year = 19;                                                                 // Year to Start (20xx)

        RTC_SetDate(RTC_Format_BCD,&RTC_dateInitStruct);                                                  // Set Date in BCD Format
	
/**************************************ALARM IMPLEMENTATION*********************************************/

	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;                        // Alarm Based on Weekday or Date Setting
	RTC_AlarmStruct.RTC_AlarmDateWeekDay = RTC_Weekday_Saturday;                                      // Alarm on Saturday
	RTC_AlarmStruct.RTC_AlarmTime.RTC_H12 = RTC_H12_PM;                                               // AM-PM Config
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours = 20;                                                     // Alarm at 20
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes = 10;                                                   // Alarm at 20:10
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds = 00;                                                   // Alarm at 20:10.00

	RTC_SetAlarm(RTC_Format_BCD,RTC_Alarm_A, &RTC_AlarmStruct);                                       // Set Alarm A in BCD Format
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);                                                                // Enable Alarm

}

uint8_t whichDay(uint8_t weekDay){                                                                        // Function that Converts Integer Values to Enumerated Constants
                                                                                                          // Based on the Weekday Register Number
	                                                                                                  // And Returns the Enumerated Value
	switch (weekDay){

	case 1:
		day = MONDAY;
		break;

	case 2:
		day = TUESDAY;
		break;

	case 3:
		day = WEDNESDAY;
		break;

	case 4:
		day = THURSDAY;
		break;

	case 5:
		day = FRIDAY;
		break;

	case 6:
		day = SATURDAY;
		break;

	case 7:
		day = SUNDAY;
		break;

	default:
		day = UNKNOWN;
		break;

	}

return day;

}
