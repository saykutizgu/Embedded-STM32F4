This example shows a debounce timer that activates upon a button press or release.

The problem with buttons is that when we press or release it, the voltage at its' output jumps up and down randomly.

To solve this problem after a rising edge is detected at the button output, a debounce timer starts.

After the requested time elapsed, (25 ms in my case) we read the output of the button again and achieved a nice robust on/off signal.

I used my STM32F401RE development board and its' GPIOA, GPIOC, TIM3, EXTI, NVIC peripherals in this example.

I also used my 8 MHz external clock as source with PLL Configuration.

