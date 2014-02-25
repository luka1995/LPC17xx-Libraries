/****************************************************************************
	Luka Penger 2013
	SC16IS752
	LPC17xx
****************************************************************************/

#include "SC16IS752.h"

extern volatile uint8_t I2C_MasterBuffer[I2C_PORT_NUM][I2C_BUFSIZE];
extern volatile uint8_t I2C_SlaveBuffer[I2C_PORT_NUM][I2C_BUFSIZE];
extern volatile uint32_t I2C_WriteLength[I2C_PORT_NUM];
extern volatile uint32_t I2C_ReadLength[I2C_PORT_NUM];

/****************************************************************************
	Functions
****************************************************************************/

char SC16IS752_Read(char portNum, char address, char channel, char regAddress) {
	I2C_WriteLength[portNum] = 2;
	I2C_ReadLength[portNum] = 0;
	I2C_MasterBuffer[portNum][0] = address;
	I2C_MasterBuffer[portNum][1] = ((regAddress << 3) | (channel	<< 1));
	I2C_Engine(portNum);
	
	I2C_WriteLength[portNum] = 1;
	I2C_ReadLength[portNum] = 1;
	I2C_MasterBuffer[portNum][0] = (address | I2C_READ_BIT);
	I2C_Engine(portNum);

	return I2C_SlaveBuffer[portNum][0];
}

void SC16IS752_Write(char portNum, char address, char channel, char regAddress, char data) {
	I2C_WriteLength[portNum] = 3;
	I2C_ReadLength[portNum] = 0;
	I2C_MasterBuffer[portNum][0] = address;
	I2C_MasterBuffer[portNum][1] = ((regAddress << 3) | (channel << 1));
	I2C_MasterBuffer[portNum][2] = data;
	I2C_Engine(portNum);
}

char SC16IS752_GPIO_Read(char portNum, char address) {
	return SC16IS752_Read(portNum, address, 0, SC16IS752_IOState);
}

void SC16IS752_GPIO_Write(char portNum, char address, char data) {
	SC16IS752_Write(portNum, address, 0, SC16IS752_IOState, data); // set GPIO Output pins state 0-7
}

void SC16IS752_GPIO_Reset(char portNum, char pinNum) {
	switch (portNum) {
		case 0:
			LPC_GPIO0->FIODIR |= (1<<pinNum); // set output
			delayMs(SC16IS752_DELAY_TIMER_NUM, SC16IS752_GPIO_RESET_DELAY_MS);
			LPC_GPIO0->FIOCLR = (1<<pinNum); // low
			delayMs(SC16IS752_DELAY_TIMER_NUM, SC16IS752_GPIO_RESET_DELAY_MS);
			LPC_GPIO0->FIOSET = (1<<pinNum); // high
			break;
		case 1:
			LPC_GPIO1->FIODIR |= (1<<pinNum); // set output
			delayMs(SC16IS752_DELAY_TIMER_NUM, SC16IS752_GPIO_RESET_DELAY_MS);
			LPC_GPIO1->FIOCLR = (1<<pinNum); // low
			delayMs(SC16IS752_DELAY_TIMER_NUM, SC16IS752_GPIO_RESET_DELAY_MS);
			LPC_GPIO1->FIOSET = (1<<pinNum); // high
			break;
		case 2:
			LPC_GPIO2->FIODIR |= (1<<pinNum); // set output
			delayMs(SC16IS752_DELAY_TIMER_NUM, SC16IS752_GPIO_RESET_DELAY_MS);
			LPC_GPIO2->FIOCLR = (1<<pinNum); // low
			delayMs(SC16IS752_DELAY_TIMER_NUM, SC16IS752_GPIO_RESET_DELAY_MS);
			LPC_GPIO2->FIOSET = (1<<pinNum); // high
			break;
		case 3:
			LPC_GPIO3->FIODIR |= (1<<pinNum); // set output
			delayMs(SC16IS752_DELAY_TIMER_NUM, SC16IS752_GPIO_RESET_DELAY_MS);
			LPC_GPIO3->FIOCLR = (1<<pinNum); // low
			delayMs(SC16IS752_DELAY_TIMER_NUM, SC16IS752_GPIO_RESET_DELAY_MS);
			LPC_GPIO3->FIOSET = (1<<pinNum); // high
			break;
		default:
			LPC_GPIO4->FIODIR |= (1<<pinNum); // set output
			delayMs(SC16IS752_DELAY_TIMER_NUM, SC16IS752_GPIO_RESET_DELAY_MS);
			LPC_GPIO4->FIOCLR = (1<<pinNum); // low
			delayMs(SC16IS752_DELAY_TIMER_NUM, SC16IS752_GPIO_RESET_DELAY_MS);
			LPC_GPIO4->FIOSET = (1<<pinNum); // high
			break;
	}
}

/****************************************************************************
	UART Write/Read
****************************************************************************/

char SC16IS752_Read_Char(char portNum, char address, char channel) {
	return SC16IS752_Read(portNum, address, channel, SC16IS752_RHR);
}

void SC16IS752_Send_Char(char portNum, char address, char channel, char data) {
	SC16IS752_Write(portNum, address, channel, SC16IS752_THR, data);
}

void SC16IS752_Send_Buffer(char portNum, char address, char channel, char *buffer, unsigned int length) {
	int a = 0;
	
	for(a=0; a<length; a++) {
		SC16IS752_Send_Char(portNum, address, channel, buffer[a]);
	}
	
	/*int i = length, a = 0;
	int next = 0;
	int len = 0;
	
	while (i > 0) {
		if(i > 64) {
			len = 64;
		} else {
			len = i;
		}
		
		for(a=0; a<len; a++) {
			SC16IS752_Send_Char(portNum, address, channel, buffer[a+next]);
		}
		
		while (!(SC16IS752_Read(portNum, address, channel, SC16IS752_LSR) & 0x20));

		next += len;
		i -= len;
	}*/
}

/****************************************************************************
	UART Type
****************************************************************************/

void SC16IS752_UART_Set_Type(char portNum, char address, char channel, SC16IS752_UART_Type type) {
	char data = SC16IS752_GPIO_Read(portNum, address);

	if(type == SC16IS752_UART_Type_RS232) {
		if(channel == SC16IS752_CHANNEL_A) {
			data &= ~(1<<0); // U1-232 = 0
			data |= (1<<1);	// U1-422 = 1
			data &= ~(1<<2); // Termination = 0
		} else {
			data &= ~(1<<3); // U1-232 = 0
			data |= (1<<4);	// U1-422 = 1
			data &= ~(1<<5); // Termination = 0
		}
	} else if(type == SC16IS752_UART_Type_RS485) {
		if(channel == SC16IS752_CHANNEL_A) {
			data |= (1<<0); // U1-232 = 1
			data |= (1<<1);	// U1-422 = 1
		} else {
			data |= (1<<3); // U1-232 = 1
			data |= (1<<4);	// U1-422 = 1
		}
	} else {
		if(channel == SC16IS752_CHANNEL_A) {
			data |= (1<<0); // U1-232 = 1
			data &= ~(1<<1);	// U1-422 = 0
		} else {
			data |= (1<<3); // U1-232 = 1
			data &= ~(1<<4);	// U1-422 = 0
		}
	}
	
	SC16IS752_GPIO_Write(portNum, address, data);
}
	
SC16IS752_UART_Type SC16IS752_UART_Read_Type(char portNum, char address, char channel) {
	char data = SC16IS752_GPIO_Read(portNum, address);

	if(channel == SC16IS752_CHANNEL_A) {
		if(!(data & 0x01) && (data & 0x02)) {
			return SC16IS752_UART_Type_RS232;
		} else if((data & 0x01) && (data & 0x02)) {
			return SC16IS752_UART_Type_RS485;
		} else {
			return SC16IS752_UART_Type_RS422;
		}
	} else {
		if(!(data & 0x08) && (data & 0x10)) {
			return SC16IS752_UART_Type_RS232;
		} else if((data & 0x08) && (data & 0x10)) {
			return SC16IS752_UART_Type_RS485;
		} else {
			return SC16IS752_UART_Type_RS422;
		}
	}
}

SC16IS752_UART_Type SC16IS752_UART_Type_From_Number(char number) {
	switch (number) {
		default: return SC16IS752_UART_Type_RS232;
		case 1: return SC16IS752_UART_Type_RS485;
		case 2: return SC16IS752_UART_Type_RS422;
	}
}

/****************************************************************************
	Disable / Enable
****************************************************************************/

void SC16IS752_TX_Disable(char portNum, char address, char channel) {
	char EFCR = SC16IS752_Read(portNum, address, channel, SC16IS752_EFCR);
	
	EFCR |= (1<<2);
	
	SC16IS752_Write(portNum, address, channel, SC16IS752_EFCR, EFCR);
}

void SC16IS752_TX_Enable(char portNum, char address, char channel) {
	char EFCR = SC16IS752_Read(portNum, address, channel, SC16IS752_EFCR);
	
	EFCR &= ~(1<<2);
	
	SC16IS752_Write(portNum, address, channel, SC16IS752_EFCR, EFCR);
}

void SC16IS752_RX_Disable(char portNum, char address, char channel) {
	char EFCR = SC16IS752_Read(portNum, address, channel, SC16IS752_EFCR);
	
	EFCR |= (1<<1);
	
	SC16IS752_Write(portNum, address, channel, SC16IS752_EFCR, EFCR);
}

void SC16IS752_RX_Enable(char portNum, char address, char channel) {
	char EFCR = SC16IS752_Read(portNum, address, channel, SC16IS752_EFCR);
	
	EFCR &= ~(1<<1);
	
	SC16IS752_Write(portNum, address, channel, SC16IS752_EFCR, EFCR);
}

void SC16IS752_Disable(char portNum, char address, char channel) {
	SC16IS752_TX_Disable(portNum, address, channel);
	SC16IS752_RX_Disable(portNum, address, channel);

	SC16IS752_RX_FIFO_Reset(portNum, address, channel);
	SC16IS752_TX_FIFO_Reset(portNum, address, channel);

	SC16IS752_FIFO_Disable(portNum, address, channel);

	SC16IS752_Sleep_Enable(portNum, address, channel);
}

void SC16IS752_Enable(char portNum, char address, char channel) {
	SC16IS752_TX_Enable(portNum, address, channel);
	SC16IS752_RX_Enable(portNum, address, channel);
	
	SC16IS752_RX_FIFO_Reset(portNum, address, channel);
	SC16IS752_TX_FIFO_Reset(portNum, address, channel);

	SC16IS752_FIFO_Enable(portNum, address, channel);

	SC16IS752_Sleep_Disable(portNum, address, channel);
}

/****************************************************************************
	UART Baud Rate
****************************************************************************/

SC16IS752_UART_Baud_Rate SC16IS752_UART_Baud_Rate_From_Value(char value) {
	switch (value) {
		case 0: return SC16IS752_UART_Baud_Rate_50;
		case 1: return SC16IS752_UART_Baud_Rate_75;
		case 2: return SC16IS752_UART_Baud_Rate_110;
		case 3: return SC16IS752_UART_Baud_Rate_135;
		case 4: return SC16IS752_UART_Baud_Rate_150;
		case 5: return SC16IS752_UART_Baud_Rate_300;
		case 6: return SC16IS752_UART_Baud_Rate_600;
		case 7: return SC16IS752_UART_Baud_Rate_1200;
		case 8: return SC16IS752_UART_Baud_Rate_1800;
		case 9: return SC16IS752_UART_Baud_Rate_2000;
		case 10: return SC16IS752_UART_Baud_Rate_2400;
		case 11: return SC16IS752_UART_Baud_Rate_3600;
		case 12: return SC16IS752_UART_Baud_Rate_4800;
		case 13: return SC16IS752_UART_Baud_Rate_7200;
		case 14: return SC16IS752_UART_Baud_Rate_9600;
		case 15: return SC16IS752_UART_Baud_Rate_19200;
		case 16: return SC16IS752_UART_Baud_Rate_38400;
		case 17: return SC16IS752_UART_Baud_Rate_56000;
		case 18: return SC16IS752_UART_Baud_Rate_115200;
		default: return SC16IS752_UART_Baud_Rate_9600;
	}
}

short SC16IS752_UART_Baud_Value(SC16IS752_UART_Baud_Rate baudRate) {
	switch (baudRate)
	{
		case SC16IS752_UART_Baud_Rate_50:
			return 2304;
		case SC16IS752_UART_Baud_Rate_75:
			return 1536;
		case SC16IS752_UART_Baud_Rate_110:
			return 1047;
		case SC16IS752_UART_Baud_Rate_135:
			return 857;
		case SC16IS752_UART_Baud_Rate_150:
			return 768;
		case SC16IS752_UART_Baud_Rate_300:
			return 384;
		case SC16IS752_UART_Baud_Rate_600:
			return 192;
		case SC16IS752_UART_Baud_Rate_1200:
			return 96;
		case SC16IS752_UART_Baud_Rate_1800:
			return 64;
		case SC16IS752_UART_Baud_Rate_2000:
			return 58;
		case SC16IS752_UART_Baud_Rate_2400:
			return 48;
		case SC16IS752_UART_Baud_Rate_3600:
			return 32;
		case SC16IS752_UART_Baud_Rate_4800:
			return 24;
		case SC16IS752_UART_Baud_Rate_7200:
			return 16;
		case SC16IS752_UART_Baud_Rate_9600:
			return 12;
		case SC16IS752_UART_Baud_Rate_19200:
			return 6;
		case SC16IS752_UART_Baud_Rate_38400:
			return 3;
		case SC16IS752_UART_Baud_Rate_56000:
			return 2;
		case SC16IS752_UART_Baud_Rate_115200:
			return 1;
		default: // SC16IS752_UART_Baud_Rate_9600
			return 12;
	}
}

void SC16IS752_UART_Set_Baud_Rate(char portNum, char address, char channel, SC16IS752_UART_Baud_Rate baudRate) {
	char IER = 0x00, LCR = 0x00;
	short value = SC16IS752_UART_Baud_Value(baudRate);
	char data1 = (value & 0xFF);
	char data2 = ((value & 0xFF00) >> 8);
	
	// DISABLE SLEEP MODE
	IER = SC16IS752_Read(portNum, address, channel, SC16IS752_IER);
	IER &= ~(1<<4);
	SC16IS752_Write(portNum, address, channel, SC16IS752_IER, IER);
	
	// Divisor latch enable
	LCR = SC16IS752_Read(portNum, address, channel, SC16IS752_LCR);
	LCR |= (1<<7);
	SC16IS752_Write(portNum, address, channel, SC16IS752_LCR, LCR);
	
	// WRITE BAUD RATE
	SC16IS752_Write(portNum, address, channel, SC16IS752_DLL, data1);
	SC16IS752_Write(portNum, address, channel, SC16IS752_DLH, data2);
	
	// Divisor latch disable
	LCR &= ~(1<<7);
	SC16IS752_Write(portNum, address, channel, SC16IS752_LCR, LCR);
}

SC16IS752_UART_Baud_Rate SC16IS752_UART_Read_Baud_Rate(char portNum, char address, char channel) {
	char data1 = SC16IS752_Read(portNum, address, channel, SC16IS752_DLL);
	char data2 = SC16IS752_Read(portNum, address, channel, SC16IS752_DLH);

	short value = (((data2 << 8) & 0xFF00) & (data1 & 0x00FF));

	return (SC16IS752_UART_Baud_Rate)value;
}

/****************************************************************************
	FIFO
****************************************************************************/

void SC16IS752_RX_FIFO_Reset(char portNum, char address, char channel) {
	char data = SC16IS752_Read(portNum, address, channel, SC16IS752_FCR);

	data |= (1<<1);

	SC16IS752_Write(portNum, address, channel, SC16IS752_FCR, data);
}

void SC16IS752_TX_FIFO_Reset(char portNum, char address, char channel) {
	char data = SC16IS752_Read(portNum, address, channel, SC16IS752_FCR);

	data |= (1<<2);

	SC16IS752_Write(portNum, address, channel, SC16IS752_FCR, data);
}

void SC16IS752_FIFO_Disable(char portNum, char address, char channel) {
	char data = SC16IS752_Read(portNum, address, channel, SC16IS752_FCR);

	data &= ~(1<<0);

	SC16IS752_Write(portNum, address, channel, SC16IS752_FCR, data);
}

void SC16IS752_FIFO_Enable(char portNum, char address, char channel) {
	char data = SC16IS752_Read(portNum, address, channel, SC16IS752_FCR);

	data |= (1<<0);

	SC16IS752_Write(portNum, address, channel, SC16IS752_FCR, data);
}

/****************************************************************************
	Sleep
****************************************************************************/

void SC16IS752_Sleep_Disable(char portNum, char address, char channel) {
	char data = SC16IS752_Read(portNum, address, channel, SC16IS752_IER);

	data &= ~(1<<4);

	SC16IS752_Write(portNum, address, channel, SC16IS752_IER, data);
}

void SC16IS752_Sleep_Enable(char portNum, char address, char channel) {
	char data = SC16IS752_Read(portNum, address, channel, SC16IS752_IER);

	data |= (1<<4);

	SC16IS752_Write(portNum, address, channel, SC16IS752_IER, data);
}

/****************************************************************************
	Init
****************************************************************************/

void SC16IS752_I2C_Init(char portNum, char address) {
	if(portNum == 0) {
		I2C0_Init(FALSE);
	} else if(portNum == 1) {
		I2C1_Init(TRUE);
	} else {
		I2C2_Init();
	}

	// Channel A Configuration
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_A, SC16IS752_LCR, 0x80); // Divisor latch enable
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_A, SC16IS752_DLL, SC16IS752_UART_Baud_Value(SC16IS752_UART_Baud_Rate_9600)); // Default baud rate 9600
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_A, SC16IS752_DLH, 0x00);
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_A, SC16IS752_LCR, 0xBF); // access EFR register 
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_A, SC16IS752_EFR, 0x00); // enable enhanced registers
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_A, SC16IS752_LCR, 0x03); // 8 data bits, 1 stop bit, no parity
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_A, SC16IS752_EFCR, 0x00); // Extra Features Control Register
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_A, SC16IS752_IER, 0x05); // Interrupt Enable Register
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_A, SC16IS752_FCR, 0x07); // reset TXFIFO, reset RXFIFO, enable FIFO mode

	// Channel B Configuration
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_B, SC16IS752_LCR, 0x80); // Divisor latch enable
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_B, SC16IS752_DLL, SC16IS752_UART_Baud_Value(SC16IS752_UART_Baud_Rate_9600)); // Default baud rate 9600
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_B, SC16IS752_DLH, 0x00);
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_B, SC16IS752_LCR, 0xBF); // access EFR register 
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_B, SC16IS752_EFR, 0x00); // enable enhanced registers
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_B, SC16IS752_LCR, 0x03); // 8 data bits, 1 stop bit, no parity
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_B, SC16IS752_EFCR, 0x00); // Extra Features Control Register
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_B, SC16IS752_IER, 0x05); // Interrupt Enable Register
	SC16IS752_Write(portNum, address, SC16IS752_CHANNEL_B, SC16IS752_FCR, 0x07); // reset TXFIFO, reset RXFIFO, enable FIFO mode

	// Configuration
	SC16IS752_Write(portNum, address, 0, SC16IS752_IOControl, 0x00); // Set the IOControl Register to GPIO Control
	SC16IS752_Write(portNum, address, 0, SC16IS752_IODir, 0xFF); // output the control bits to the IO Direction Register
	SC16IS752_GPIO_Write(portNum, address, 0xFF);

	SC16IS752_UART_Set_Type(portNum, address, SC16IS752_CHANNEL_A, SC16IS752_UART_Type_RS232); // Default RS232
	SC16IS752_UART_Set_Type(portNum, address, SC16IS752_CHANNEL_B, SC16IS752_UART_Type_RS232); // Default RS232
}

void SC16IS752_GPIO2_Interrupt_Init(char pin) {
	LPC_GPIOINT->IO2IntEnF |= (1<<pin);	// Enable Falling
	NVIC_EnableIRQ(EINT3_IRQn);
}

/****************************************************************************
                            End Of File
****************************************************************************/
