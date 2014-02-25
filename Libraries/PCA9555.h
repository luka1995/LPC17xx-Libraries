/****************************************************************************
	Luka Penger 2013
	PCA9555
	LPC17xx
****************************************************************************/
#ifndef __PCA9555_H 
#define __PCA9555_H

#include <LPC17xx.H>
#include "i2c.h"

void PCA9555_I2C_Init(char portNum, char address);
char PCA9555_I2C_ReadInputPort0(char portNum, char address);
char PCA9555_I2C_ReadInputPort1(char portNum, char address);
void PCA9555_I2C_WriteOutputPort0(char portNum, char address, char value);
void PCA9555_I2C_WriteOutputPort1(char portNum, char address, char value);
void PCA9555_I2C_WriteRegister(char portNum, char address, char regAdd, char value);

#endif /* end __PCA9555_H */
/***************************************************************************
                            End Of File
***************************************************************************/
