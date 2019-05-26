#include "stm32f4xx.h"

#define slaveAdd 0xD0 												 // Slave address of mpu6050 with (0x68 << 1) operation already done for i2c_bus_start

uint8_t gyro_XH = 0;  												 // Gyroscope High Value for X
uint8_t gyro_XL = 0;  												 // Gyroscope Low Value for X

uint8_t rawGyro = 0;												 // Gyroscope Raw Value

void initHw();
void i2c_bus_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void i2c_bus_write(I2C_TypeDef* I2Cx, uint8_t data);
uint8_t i2c_bus_read_ack(I2C_TypeDef* I2Cx);
uint8_t i2c_bus_read_nack(I2C_TypeDef* I2Cx);
void i2c_bus_stop(I2C_TypeDef* I2Cx);


int main(void){

initHw();

while (1)
  {
	i2c_bus_start(I2C1, slaveAdd, I2C_Direction_Transmitter); 	 	 // Starting transmission in Master transmitter mode
	i2c_bus_write(I2C1, 0x6B); 								  	 	 // Write one byte to the slave, PWR_MGMT_1 register, MPU-6050 power up
	i2c_bus_write(I2C1, 0x00); 								  	 	 // Write one byte to the slave, set to zero according to register config
	i2c_bus_stop(I2C1); 								  	  	 	 // Stopping transmission

	i2c_bus_start(I2C1, slaveAdd, I2C_Direction_Transmitter); 	 	 // Starting transmission in Master transmitter mode
	i2c_bus_write(I2C1, 0x1B); 								  	 	 // Write one byte to the slave, GYRO_CONFIG register, Gyro configuration
	i2c_bus_write(I2C1, 0x00); 								  	 	 // Write one byte to the slave, set to zero according to register config
	i2c_bus_stop(I2C1); 								      		 // Stop the transmission

	i2c_bus_start(I2C1, slaveAdd, I2C_Direction_Transmitter); 		 // Starting transmission in Master transmitter mode
	i2c_bus_write(I2C1, 0x43); 								 	     // Write one byte to the slave, GYRO_XH_OUT register, For reading Gyro X High Value
	i2c_bus_stop(I2C1); 								     		 // Stop the transmission
	i2c_bus_start(I2C1, slaveAdd, I2C_Direction_Receiver);   		 // Starting transmission in Master receiver mode
	gyro_XH = i2c_bus_read_nack(I2C1);						  		 // Reading Gyro X High

	i2c_bus_start(I2C1, slaveAdd, I2C_Direction_Transmitter); 		 // Starting transmission in Master transmitter mode
	i2c_bus_write(I2C1, 0x44); 								 		 // Write one byte to the slave, GYRO_XL_OUT register, For reading Gyro X Low Value
	i2c_bus_stop(I2C1); 									 		 // Stop the transmission
	i2c_bus_start(I2C1, slaveAdd, I2C_Direction_Receiver);   		 // Starting transmission in Master receiver mode
	gyro_XL = i2c_bus_read_nack(I2C1);						 		 // Reading Gyro X Low

	rawGyro = (gyro_XH << 8) | gyro_XL;								 // Shifting Gyro X high to 8 bits left, putting Gyro X Low to empty bits

	asm("NOP");

  }
}
void initHw(){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);    		 // Enabling System Clock For I2C1 & I2C1's GPIO Peripheral
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_InitTypeDef GPIO_initStruct;
	I2C_InitTypeDef I2C_initStruct;

	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF;				 		 // I2C1's GPIO Peripheral Configurations
	GPIO_initStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	GPIO_Init(GPIOB, &GPIO_initStruct);

	I2C_initStruct.I2C_Ack = I2C_Ack_Disable;				 		 // I2C1 Configurations
	I2C_initStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_initStruct.I2C_ClockSpeed = 400000;
	I2C_initStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_initStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_initStruct.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C1, &I2C_initStruct);
	I2C_Cmd(I2C1, ENABLE);

}

void i2c_bus_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
{

	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));			          // Wait until I2Cx is not busy anymore

	I2C_GenerateSTART(I2Cx, ENABLE);                                  // Send I2Cx START condition

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));       // Wait until I2Cx selected Master Mode

	I2C_Send7bitAddress(I2Cx, address, direction);                    // Send Slave Address to write

	if(direction == I2C_Direction_Transmitter){					      // Selection of Master Mode according to direction

		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if(direction == I2C_Direction_Receiver){

		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}
void i2c_bus_write(I2C_TypeDef* I2Cx, uint8_t data)
{
	I2C_SendData(I2Cx, data);									      // Sending Data

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  // Wait until data is transmitted
}

uint8_t i2c_bus_read_ack(I2C_TypeDef* I2Cx){

	I2C_AcknowledgeConfig(I2Cx, ENABLE);							  // Enabling acknowledge of received data

	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );	  // Wait until data is received to I2Cx

	uint8_t data = I2C_ReceiveData(I2Cx);                             // Read the data from I2Cx

	return data;
}
uint8_t i2c_bus_read_nack(I2C_TypeDef* I2Cx){

	I2C_AcknowledgeConfig(I2Cx, DISABLE);							  // Disabling acknowledge of received data

	I2C_GenerateSTOP(I2Cx, ENABLE);									  // Send I2Cx STOP condition

	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );	  // Wait until data is recieved to I2Cx

	uint8_t data = I2C_ReceiveData(I2Cx); 							  // Read the data from I2Cx

	return data;
}
void i2c_bus_stop(I2C_TypeDef* I2Cx){

	I2C_GenerateSTOP(I2Cx, ENABLE);									  // Send I2Cx STOP condition
}
