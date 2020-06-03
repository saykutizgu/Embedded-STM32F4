This is an Output Compare example that I built and tested with my STM32F401RE development board.

Timer 3 Channels 1, 2 and 3 are used to create different outputs.

In Timer 3 Channel 1, output gets toggled when Timer Counter value is equal to Timer Pulse value.

In Timer 3 Channel 2, output gets high when Timer Counter value < Timer Pulse value. (In Upcounting Mode)

In Timer 3 Channel 3, outputs gets high when Timer Counter value > Timer Pulse value. (Active Config)

When Timer 3 Channel 3 is high, I change the mode of Timer 3 Channel 3 Output Compare Mode from Active to Deactive and set a new pulse mode.

Now the output gets low Timer Counter value > Timer Pulse value. (Inactive Config)

Timer 2 Channel 1 is used in Input Capture mode to detect a falling edge on Timer 3 Channel 3 output, and change Timer 3 Channel 3's Output Compare pulse value and operation mode (Changes From Inactive -> Active In Timer 2 Interrupt).

This way we get a custom square wave output from Timer 3 Channel 3 Output Using 2 Timers one in Input Capture and one in Output Compare.
