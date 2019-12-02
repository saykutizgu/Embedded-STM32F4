This is an example of DMA (Direct Memory Access) Peripheral.
I am transferring a string data in my microcontroller to the USART2 peripheral to my computer's terminal viewer through the USB port.
Normally, if a peripheral requests data, the data flows from memory to CPU to peripheral which requires 2 steps of operation.
The DMA peripheral is provides us to flow data from memory to peripheral directly which only requires 1 step of operation.
Check the code comments for further information.
