/****************************************************************************
	Luka Penger 2013
	PCA9555
	LPC17xx
****************************************************************************/

#include "PCA9555.h"

extern volatile uint8_t I2C_MasterBuffer[I2C_PORT_NUM][I2C_BUFSIZE];
extern volatile uint8_t I2C_SlaveBuffer[I2C_PORT_NUM][I2C_BUFSIZE];
extern volatile uint32_t I2C_WriteLength[I2C_PORT_NUM];
extern volatile uint32_t I2C_ReadLength[I2C_PORT_NUM];

void PCA9555_I2C_Init(char portNum, char address) {
	switch (portNum) {
		case 0:
		I2C0_Init(FALSE);
			break;
		case 1:
		I2C1_Init(FALSE);
			break;
		default:
		I2C2_Init();
			break;
	}
	
	// PCA9555 Configuration
	PCA9555_I2C_WriteRegister(portNum, address, 0x04, 0x00); // Port 0
	PCA9555_I2C_WriteRegister(portNum, address, 0x05, 0x00); // Port 1

	//	Register 2 (Polarity Inversion Register)
	PCA9555_I2C_WriteRegister(portNum, address, 0x06, 0x00);	// Port 0
	PCA9555_I2C_WriteRegister(portNum, address, 0x07, 0x00);	// Port 1

	// Output
	PCA9555_I2C_WriteRegister(portNum, address, 0x02, 0xFF);	// Port 0
	PCA9555_I2C_WriteRegister(portNum, address, 0x03, 0xFF);	// Port 1
}

char PCA9555_I2C_ReadInputPort0(char portNum, char address) {
	I2C_WriteLength[portNum] = 2;
	I2C_ReadLength[portNum] = 0;
	I2C_MasterBuffer[portNum][0] = address;	// I2C Address
	I2C_MasterBuffer[portNum][1] = 0x00;	// Register					
	I2C_Engine(portNum);
	
	I2C_WriteLength[portNum] = 1;
	I2C_ReadLength[portNum] = 1;
	I2C_MasterBuffer[portNum][0] = (address | I2C_READ_BIT);	// I2C Address			
	I2C_Engine(portNum);
	
	return (char)I2C_SlaveBuffer[portNum][0];
}

char PCA9555_I2C_ReadInputPort1(char portNum, char address) {
	I2C_WriteLength[portNum] = 2;
	I2C_ReadLength[portNum] = 0;
	I2C_MasterBuffer[portNum][0] = address;	// I2C Address
	I2C_MasterBuffer[portNum][1] = 0x01;	// Register					
	I2C_Engine(portNum);
	
	I2C_WriteLength[portNum] = 1;
	I2C_ReadLength[portNum] = 1;
	I2C_MasterBuffer[portNum][0] = (address | I2C_READ_BIT);	// I2C Address			
	I2C_Engine(portNum);
	
	return (char)I2C_SlaveBuffer[portNum][0];
}

void PCA9555_I2C_WriteOutputPort0(char portNum, char address, char value) {
	PCA9555_I2C_WriteRegister(portNum, address, 0x02, value);	// Output Register
}

void PCA9555_I2C_WriteOutputPort1(char portNum, char address, char value) {
	PCA9555_I2C_WriteRegister(portNum, address, 0x03, value);	// Output Register
}

void PCA9555_I2C_WriteRegister(char portNum, char address, char regAdd, char value) {
	I2C_WriteLength[portNum] = 3;
	I2C_ReadLength[portNum] = 0;
	I2C_MasterBuffer[portNum][0] = address;	// I2C Address
	I2C_MasterBuffer[portNum][1] = regAdd;	// Register								 
	I2C_MasterBuffer[portNum][2] = value;	// Configuration port		
	I2C_Engine(portNum);
}
	
/**************************************************************************
                            End Of File
***************************************************************************/
