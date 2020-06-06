In this example I have used my STM32F401RE development board's internal hardware Timer 2, on-board LED and the GPIOA peripheral.

This code simply detects a rising edge on the input signal of the Timer 2 Channel 2 GPIO pins and enters into an interrupt.

In each detection, the on-board LED output gets toggled. We could also detect falling edge or both edges with this method.
