/****************************************************************************
	Luka Penger 2013
	SC16IS752
	LPC17xx
****************************************************************************/
#ifndef __SC16IS752_H 
#define __SC16IS752_H

#include <LPC17xx.H>
#include "I2C.h"
#include "DELAY.h"

#define SC16IS752_BUFFER_SIZE						100

#define SC16IS752_CHANNEL_A 						0x00
#define SC16IS752_CHANNEL_B 						0x01

#define SC16IS752_RHR          					0x00 //  Recv Holding Register is 0x00 in READ Mode 
#define SC16IS752_THR          					0x00 //  Xmit Holding Register is 0x00 in WRITE Mode 

#define SC16IS752_IER          					0x01  // Interrupt Enable Register 

#define SC16IS752_IIR          					0x02  // Interrupt Identification Register in READ Mode 
#define SC16IS752_FCR          					0x02  // FIFO Control Register in WRITE Mode 

#define SC16IS752_LCR          					0x03  // Line Control Register 
#define SC16IS752_MCR          					0x04  // Modem Control Register 
#define SC16IS752_LSR          					0x05  // Line status Register 
#define SC16IS752_MSR          					0x06  // Modem Status Register 
#define SC16IS752_SPR          					0x07  // ScratchPad Register 
#define SC16IS752_TCR          					0x06  // Transmission Control Register 
#define SC16IS752_TLR          					0x07  // Trigger Level Register 
#define SC16IS752_TXLVL        					0x08  // Xmit FIFO Level Register 
#define SC16IS752_RXLVL        					0x09  // Recv FIFO Level Register 
#define SC16IS752_IODir        					0x0A  // I/O P:ins Direction Register 
#define SC16IS752_IOState      					0x0B  // I/O Pins State Register 
#define SC16IS752_IOIntEna     					0x0C  // I/O Interrupt Enable Register 
#define SC16IS752_IOControl    					0x0E  // I/O Pins Control Register 
#define SC16IS752_EFCR         					0x0F  // Extra Features Control Register 

#define SC16IS752_DLL         					0x00  // Divisor Latch LSB  0x00 
#define SC16IS752_DLH          					0x01  // Divisor Latch MSB  0x01 

#define SC16IS752_EFR          					0x02  // Enhanced Function Register 

#define SC16IS752_DELAY_TIMER_NUM 			0
#define SC16IS752_GPIO_RESET_DELAY_MS		20

typedef enum SC16IS752_UART_Type {
	SC16IS752_UART_Type_RS232 = 0,
	SC16IS752_UART_Type_RS485 = 1,
	SC16IS752_UART_Type_RS422 = 2
} SC16IS752_UART_Type;

typedef enum SC16IS752_UART_Baud_Rate {
	SC16IS752_UART_Baud_Rate_50 = 0,
	SC16IS752_UART_Baud_Rate_75 = 1,
	SC16IS752_UART_Baud_Rate_110 = 2,
	SC16IS752_UART_Baud_Rate_135 = 3,
	SC16IS752_UART_Baud_Rate_150 = 4,
	SC16IS752_UART_Baud_Rate_300 = 5,
	SC16IS752_UART_Baud_Rate_600 = 6,
	SC16IS752_UART_Baud_Rate_1200 = 7,
	SC16IS752_UART_Baud_Rate_1800 = 8,
	SC16IS752_UART_Baud_Rate_2000 = 9,
	SC16IS752_UART_Baud_Rate_2400 = 10,
	SC16IS752_UART_Baud_Rate_3600 = 11,
	SC16IS752_UART_Baud_Rate_4800 = 12,
	SC16IS752_UART_Baud_Rate_7200 = 13,
	SC16IS752_UART_Baud_Rate_9600 = 14,
	SC16IS752_UART_Baud_Rate_19200 = 15,
	SC16IS752_UART_Baud_Rate_38400 = 16,
	SC16IS752_UART_Baud_Rate_56000 = 17,
	SC16IS752_UART_Baud_Rate_115200 = 18
} SC16IS752_UART_Baud_Rate;

/****************************************************************************
	Functions
****************************************************************************/

char SC16IS752_Read(char portNum, char address, char channel, char regAddress);
void SC16IS752_Write(char portNum, char address, char channel, char regAddress, char data);
char SC16IS752_GPIO_Read(char portNum, char address);
void SC16IS752_GPIO_Write(char portNum, char address, char data);
void SC16IS752_GPIO_Reset(char portNum, char pinNum);

/****************************************************************************
	UART Write/Read
****************************************************************************/

char SC16IS752_Read_Char(char portNum, char address, char channel);
void SC16IS752_Send_Char(char portNum, char address, char channel, char data);
void SC16IS752_Send_Buffer(char portNum, char address, char channel, char *buffer, unsigned int length);

/****************************************************************************
	UART Type
****************************************************************************/

void SC16IS752_UART_Set_Type(char portNum, char address, char channel, SC16IS752_UART_Type type);
SC16IS752_UART_Type SC16IS752_UART_Read_Type(char portNum, char address, char channel);
SC16IS752_UART_Type SC16IS752_UART_Type_From_Number(char number);

/****************************************************************************
	Disable / Enable
****************************************************************************/

void SC16IS752_TX_Disable(char portNum, char address, char channel);
void SC16IS752_TX_Enable(char portNum, char address, char channel);

void SC16IS752_RX_Disable(char portNum, char address, char channel);
void SC16IS752_RX_Enable(char portNum, char address, char channel);

void SC16IS752_Disable(char portNum, char address, char channel);
void SC16IS752_Enable(char portNum, char address, char channel);

/****************************************************************************
	UART Baud Rate
****************************************************************************/

SC16IS752_UART_Baud_Rate SC16IS752_UART_Baud_Rate_From_Value(char value);
short SC16IS752_UART_Baud_Value(SC16IS752_UART_Baud_Rate baudRate);
void SC16IS752_UART_Set_Baud_Rate(char portNum, char address, char channel, SC16IS752_UART_Baud_Rate baudRate);
SC16IS752_UART_Baud_Rate SC16IS752_UART_Read_Baud_Rate(char portNum, char address, char channel);

/****************************************************************************
	FIFO
****************************************************************************/

void SC16IS752_RX_FIFO_Reset(char portNum, char address, char channel);
void SC16IS752_TX_FIFO_Reset(char portNum, char address, char channel);

void SC16IS752_FIFO_Disable(char portNum, char address, char channel);
void SC16IS752_FIFO_Enable(char portNum, char address, char channel);

/****************************************************************************
	Sleep
****************************************************************************/

void SC16IS752_Sleep_Disable(char portNum, char address, char channel);
void SC16IS752_Sleep_Enable(char portNum, char address, char channel);

/****************************************************************************
	Init
****************************************************************************/

void SC16IS752_I2C_Init(char portNum, char address);
void SC16IS752_GPIO2_Interrupt_Init(char pin);

#endif /* end __SC16IS752_H */
/***************************************************************************
                            End Of File
***************************************************************************/
