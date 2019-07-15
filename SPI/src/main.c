#include "stm32f4xx.h"
#include "stdio.h"

void initHw(void);

void mySPI_Tx(uint8_t address, uint8_t data);
uint8_t mySPI_Rx(uint8_t address);

uint8_t receivedByte;

int main(void)
{
    initHw();

    while (1)
    {
        receivedByte = mySPI_Rx(0x00);                                                                   // Read Device ID Register
	asm("nop");
    }
}

void initHw(void){

	RCC->CFGR = 0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);                                             // SPI3 Clock Enabled
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOB, ENABLE);                     // GPIOC&B Clock Enabled

	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitTypeDef GPIO_initStructure;


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                                     // SPI3 SCK, MISO, MOSI Pins Configuration
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                                   // Alternate Function Mode, Push-Pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 ;                          // SPI3 Pins
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;                                                 // No Pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10 | GPIO_PinSource11 | GPIO_PinSource12, GPIO_AF_SPI3);   // Alternate Function Config
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_initStructure.GPIO_Mode = GPIO_Mode_OUT;                                                    // SPI3 NSS Pin Configuration
	GPIO_initStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_initStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;                               // SPI3 Configuration
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                                                    // 2 Lines Full Duplex, Master Mode
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                                                // 8-bit Data Size
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                                                      // Clock Polarity High
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;                              // SPI Baud Rate
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                                                     // Clock Active Edge for Bit Capture
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                                               // Most Significant Bit First
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                                                        // Software Slave Select
	SPI_Init(SPI3, &SPI_InitStructure);
	SPI_Cmd(SPI3, ENABLE);
}

void mySPI_Tx(uint8_t address, uint8_t data){

    GPIO_ResetBits(GPIOB, GPIO_Pin_12);                                                                  // Activate the SS pin

    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}                                     // Wait until TX Buffer is not empty

	SPI_I2S_SendData(SPI3, address);                                                                 // Send Register Address

	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET){}                                // Wait until RX Buffer is empty

	SPI_I2S_ReceiveData(SPI3);                                                                       // Dummy Read

	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}                                 // Wait until TX Buffer is not empty

	SPI_I2S_SendData(SPI3, data);                                                                    // Send Desired Data

	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET){}                                // Wait until RX Buffer is empty

	SPI_I2S_ReceiveData(SPI3);                                                                       // Dummy Read

	GPIO_SetBits(GPIOB, GPIO_Pin_12);                                                                // Deactivate the SS Pin

}

uint8_t mySPI_Rx(uint8_t address){

	uint8_t rxbyte;

    GPIO_ResetBits(GPIOB, GPIO_Pin_12);                                                                  // Activate the SS pin

    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}                                      // Wait until TX Buffer is not empty

    SPI_I2S_SendData(SPI3, 0x80 | address);                                                              // Send Register Address

	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET){}                                 // Wait until RX Buffer is empty

	SPI_I2S_ReceiveData(SPI3);                                                                       // Dummy Read

    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}                                      // Wait until TX Buffer is not empty

    SPI_I2S_SendData(SPI3, 0xF0);                                                                        // Dummy Write

	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET){}                                 // Wait until RX Buffer is empty

	rxbyte = SPI_I2S_ReceiveData(SPI3);                                                              // Receive Desired Data

	GPIO_SetBits(GPIOB, GPIO_Pin_12);                                                                // Deactivate the SS Pin

	return rxbyte;
}

