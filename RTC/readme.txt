This is an example of Real Time Clock peripheral low level driver.
In the code firstly we gained access to RTC registers which are locked initially.
Later on, I configurated predividers for my microcontroller, the STM32F401RE accordingly to the datasheet.
Then, I configured the Time, Date and Alarm registers to real time values according to my GMT. (Greenwich Mean Time)
And my real time clock is done.
