/****************************************************************************
	Luka Penger 2014
	BMP180
	LPC17xx
****************************************************************************/

#include "BMP180.h"

extern volatile uint8_t I2C_MasterBuffer[I2C_PORT_NUM][I2C_BUFSIZE];
extern volatile uint8_t I2C_SlaveBuffer[I2C_PORT_NUM][I2C_BUFSIZE];
extern volatile uint32_t I2C_WriteLength[I2C_PORT_NUM];
extern volatile uint32_t I2C_ReadLength[I2C_PORT_NUM];

int AC1,AC2,AC3,VB1,VB2,MB,MC,MD;
unsigned int AC4,AC5,AC6;
double c5,c6,mc,md,x0,x1,x2,Y0,Y1,Y2,p0,p1,p2;

char BMP180_Init(void) {
	double c3,c4,b1;
	
	if (BMP180_I2C_PORT_NUM == 0) {
		I2C0_Init(FALSE);
	} else if (BMP180_I2C_PORT_NUM == 1) {
		I2C1_Init(FALSE);
	} else {
		I2C2_Init();
	}

	if (BMP180_I2C_ReadRegister(BMP180_ID_REGISTER) == BMP180_ID) {
		if (BMP180_ReadInt(0xAA, &AC1) &&
		BMP180_ReadInt(0xAC, &AC2) &&
		BMP180_ReadInt(0xAE, &AC3) &&
		BMP180_ReadUInt(0xB0, &AC4) &&
		BMP180_ReadUInt(0xB2, &AC5) &&
		BMP180_ReadUInt(0xB4, &AC6) &&
		BMP180_ReadInt(0xB6, &VB1) &&
		BMP180_ReadInt(0xB8, &VB2) &&
		BMP180_ReadInt(0xBA, &MB) &&
		BMP180_ReadInt(0xBC, &MC) &&
		BMP180_ReadInt(0xBE, &MD)) {
			// All reads completed successfully!

			// Compute floating-point polynominals

			c3 = 160.0 * pow(2,-15) * AC3;
			c4 = pow(10,-3) * pow(2,-15) * AC4;
			b1 = pow(160,2) * pow(2,-30) * VB1;
			c5 = (pow(2,-15) / 160) * AC5;
			c6 = AC6;
			mc = (pow(2,11) / pow(160,2)) * MC;
			md = MD / 160.0;
			x0 = AC1;
			x1 = 160.0 * pow(2,-13) * AC2;
			x2 = pow(160,2) * pow(2,-25) * VB2;
			Y0 = c4 * pow(2,15);
			Y1 = c4 * c3;
			Y2 = c4 * b1;
			p0 = (3791.0 - 8.0) / 1600.0;
			p1 = 1.0 - 7357.0 * pow(2,-20);
			p2 = 3038.0 * 100.0 * pow(2,-36);
			
			return 1;
		}
	}
	
	return 0;
}

char BMP180_StartTemperature(void) {
	unsigned char data[2], result;

	data[0] = BMP180_REG_CONTROL;
	data[1] = BMP180_COMMAND_TEMPERATURE;

	result = BMP180_I2C_WriteBytes(data, 2);
	if (result)	{
		return 1;
	} else {
		return 0; // or return 0 if there was a problem communicating with the BMP
	}
}

char BMP180_GetTemperature(double *T) {
	char result;
	double tu, a;

	result = BMP180_I2C_ReadBytes(BMP180_REG_RESULT, 2);
	if (result) {
		tu = (I2C_SlaveBuffer[BMP180_I2C_PORT_NUM][0] * 256.0) + I2C_SlaveBuffer[BMP180_I2C_PORT_NUM][1];

		a = c5 * (tu - c6);
		*T = a + (mc / (a + md));
	}

	return result;
}

char BMP180_StartPressure(char oversampling) {
	unsigned char data[2], result, delay;

	data[0] = BMP180_REG_CONTROL;

	switch (oversampling) {
		case 1:
			data[1] = BMP180_COMMAND_PRESSURE1;
			delay = 20;
		break;
		case 2:
			data[1] = BMP180_COMMAND_PRESSURE2;
			delay = 30;
		break;
		case 3:
			data[1] = BMP180_COMMAND_PRESSURE3;
			delay = 40;
		break;
		default:
			data[1] = BMP180_COMMAND_PRESSURE0;
			delay = 10;
		break;
	}
	
	result = BMP180_I2C_WriteBytes(data, 2);
	if (result == I2C_OK)
		return delay;
	else
		return 0; // or return 0 if there was a problem communicating with the BMP
}

char BMP180_GetPressure(double *P, double T) {
	char result;
	double pu,s,x,y,z;

	result = BMP180_I2C_ReadBytes(BMP180_REG_RESULT, 3);
	if (result) {
		pu = (I2C_SlaveBuffer[BMP180_I2C_PORT_NUM][0] * 256.0) + I2C_SlaveBuffer[BMP180_I2C_PORT_NUM][1] + (I2C_SlaveBuffer[BMP180_I2C_PORT_NUM][2]/256.0);

		s = T - 25.0;
		x = (x2 * pow(s,2)) + (x1 * s) + x0;
		y = (Y2 * pow(s,2)) + (Y1 * s) + Y0;
		z = (pu - x) / y;
		*P = (p2 * pow(z,2)) + (p1 * z) + p0;
	}
	
	return result;
}

double BMP180_Sealevel(double P, double A) {
	return (P/pow(1-(A/44330.0),5.255));
}

double BMP180_Altitude(double P, double P0) {
	return (44330.0*(1-pow(P/P0,1/5.255)));
}

char BMP180_ReadUInt(unsigned char address, unsigned int *value) {
	if (BMP180_I2C_ReadBytes(address, 2) == I2C_OK) {
		*value = (((unsigned int)I2C_SlaveBuffer[BMP180_I2C_PORT_NUM][0]<<8) | (unsigned int)I2C_SlaveBuffer[BMP180_I2C_PORT_NUM][1]);

		return 1;
	}
	
	value = 0;
	
	return 0;
}


char BMP180_ReadInt(unsigned char address, int *value) {
	if (BMP180_I2C_ReadBytes(address, 2) == I2C_OK) {
		*value = (((int)I2C_SlaveBuffer[BMP180_I2C_PORT_NUM][0]<<8) | (int)I2C_SlaveBuffer[BMP180_I2C_PORT_NUM][1]);
		if (*value & 0x8000) *value |= 0xFFFF0000; // sign extend if negative
		return 1;
	}
	
	value = 0;
	
	return 0;
}

unsigned int BMP180_I2C_WriteRegister(char regAdd, char value) {
	I2C_WriteLength[BMP180_I2C_PORT_NUM] = 3;
	I2C_ReadLength[BMP180_I2C_PORT_NUM] = 0;
	I2C_MasterBuffer[BMP180_I2C_PORT_NUM][0] = BMP180_ADDRESS;	// I2C Address
	I2C_MasterBuffer[BMP180_I2C_PORT_NUM][1] = regAdd;	// Register								 
	I2C_MasterBuffer[BMP180_I2C_PORT_NUM][2] = value;	// Configuration port		
	return I2C_Engine(BMP180_I2C_PORT_NUM);
}

char BMP180_I2C_ReadRegister(char regAdd) {
	I2C_WriteLength[BMP180_I2C_PORT_NUM] = 2;
	I2C_ReadLength[BMP180_I2C_PORT_NUM] = 0;
	I2C_MasterBuffer[BMP180_I2C_PORT_NUM][0] = BMP180_ADDRESS;	// I2C Address
	I2C_MasterBuffer[BMP180_I2C_PORT_NUM][1] = regAdd;	// Register					
	I2C_Engine(BMP180_I2C_PORT_NUM);
	
	I2C_WriteLength[BMP180_I2C_PORT_NUM] = 1;
	I2C_ReadLength[BMP180_I2C_PORT_NUM] = 1;
	I2C_MasterBuffer[BMP180_I2C_PORT_NUM][0] = (BMP180_ADDRESS | I2C_READ_BIT);	// I2C Address			
	I2C_Engine(BMP180_I2C_PORT_NUM);
	
	return (char)I2C_SlaveBuffer[BMP180_I2C_PORT_NUM][0];
}

unsigned int BMP180_I2C_WriteBytes(unsigned char *values, char length) {
	int i=0;
	
	I2C_WriteLength[BMP180_I2C_PORT_NUM] = (length + 1);
	I2C_ReadLength[BMP180_I2C_PORT_NUM] = 0;
	
	I2C_MasterBuffer[BMP180_I2C_PORT_NUM][0] = BMP180_ADDRESS;	// I2C Address
	
	for(i=0; i<length; i++) {
		I2C_MasterBuffer[BMP180_I2C_PORT_NUM][i+1] = values[i];
	}
		
	I2C_Engine(BMP180_I2C_PORT_NUM);

	return I2C_Engine(BMP180_I2C_PORT_NUM);
}

unsigned int BMP180_I2C_ReadBytes(char regAdd, char number) {
	I2C_WriteLength[BMP180_I2C_PORT_NUM] = 2;
	I2C_ReadLength[BMP180_I2C_PORT_NUM] = 0;
	I2C_MasterBuffer[BMP180_I2C_PORT_NUM][0] = BMP180_ADDRESS;	// I2C Address
	I2C_MasterBuffer[BMP180_I2C_PORT_NUM][1] = regAdd;	// Register					
	I2C_Engine(BMP180_I2C_PORT_NUM);
	
	I2C_WriteLength[BMP180_I2C_PORT_NUM] = 1;
	I2C_ReadLength[BMP180_I2C_PORT_NUM] = number;
	I2C_MasterBuffer[BMP180_I2C_PORT_NUM][0] = (BMP180_ADDRESS | I2C_READ_BIT);	// I2C Address			

	return I2C_Engine(BMP180_I2C_PORT_NUM);
}

/****************************************************************************
                            End Of File
****************************************************************************/
