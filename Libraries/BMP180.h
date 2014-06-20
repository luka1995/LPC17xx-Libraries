/****************************************************************************
	Luka Penger 2014
	BMP180
	LPC17xx
****************************************************************************/
#ifndef __BMP180_H 
#define __BMP180_H

#include <LPC17xx.h>
#include <math.h>
#include <stdio.h>
#include <I2C.h>

#define BMP180_I2C_PORT_NUM														0

#define BMP180_ADDRESS 																0xEE // 8 bit

#define	BMP180_REG_CONTROL 														0xF4
#define	BMP180_REG_RESULT 														0xF6

#define	BMP180_COMMAND_TEMPERATURE									 	0x2E
#define	BMP180_COMMAND_PRESSURE0 											0x34
#define	BMP180_COMMAND_PRESSURE1 											0x74
#define	BMP180_COMMAND_PRESSURE2 											0xB4
#define	BMP180_COMMAND_PRESSURE3 											0xF4

#define BMP180_ID_REGISTER 														0xD0
#define BMP180_ID 																		0x55

char BMP180_Init(void);

char BMP180_StartTemperature(void);
// command BMP180 to start a temperature measurement
// returns 1 for success, 0 for fail

char BMP180_GetTemperature(double *T);
// return temperature measurement from previous startTemperature command
// places returned value in T variable (deg C)
// returns 1 for success, 0 for fail

char BMP180_StartPressure(char oversampling);
// command BMP180 to start a pressure measurement
// oversampling: 0 - 3 for oversampling value
// returns (number of ms to wait) for success, 0 for fail

char BMP180_GetPressure(double *P, double T);
// return absolute pressure measurement from previous startPressure command
// note: requires previous temperature measurement in variable T
// places returned value in P variable (mbar)
// returns 1 for success, 0 for fail

double BMP180_Sealevel(double P, double A);
// convert absolute pressure to sea-level pressure (as used in weather data)
// P: absolute pressure (mbar)
// A: current altitude (meters)
// returns sealevel pressure in mbar

double BMP180_Altitude(double P, double P0);
// convert absolute pressure to altitude (given baseline pressure; sea-level, runway, etc.)
// P: absolute pressure (mbar)
// P0: fixed baseline pressure (mbar)
// returns signed altitude in meters

char BMP180_ReadUInt(unsigned char address, unsigned int *value);
char BMP180_ReadInt(unsigned char address, int *value);
unsigned int BMP180_I2C_WriteRegister(char regAdd, char value);
char BMP180_I2C_ReadRegister(char regAdd);
unsigned int BMP180_I2C_WriteBytes(unsigned char *values, char length);
unsigned int BMP180_I2C_ReadBytes(char regAdd, char number);

#endif /* end __BMP180_H */
/***************************************************************************
                            End Of File
***************************************************************************/
