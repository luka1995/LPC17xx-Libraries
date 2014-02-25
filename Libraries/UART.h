/****************************************************************************
	Luka Penger 2013
	UART
	LPC17xx
****************************************************************************/
#ifndef __UART_H 
#define __UART_H

#include <LPC17xx.H>
#include "type.h"

#define UART_BUFSIZE 		64

#define UART_CLK        	100000000     /* Serial Peripheral Clock is 100 MHz */

#define UART_IER_RBR		0x01
#define UART_IER_THRE		0x02
#define UART_IER_RLS		0x04

#define UART_IIR_PEND		0x01
#define UART_IIR_RLS		0x03
#define UART_IIR_RDA		0x02
#define UART_IIR_CTI		0x06
#define UART_IIR_THRE		0x01

#define UART_LSR_RDR		0x01
#define UART_LSR_OE			0x02
#define UART_LSR_PE			0x04
#define UART_LSR_FE			0x08
#define UART_LSR_BI			0x10
#define UART_LSR_THRE		0x20
#define UART_LSR_TEMT		0x40
#define UART_LSR_RXFE		0x80

/* Parity enumerator */
typedef enum {
  UART_PARITY_NONE    = 0,
  UART_PARITY_ODD     = 1,
  UART_PARITY_EVEN    = 2,
  UART_PARITY_MARK    = 3,
  UART_PARITY_SPACE   = 4
} UART_Parity;

/* Stop Bits enumerator */
typedef enum {
  UART_STOP_BITS_1    = 0,
  UART_STOP_BITS_1_5  = 1,
  UART_STOP_BITS_2    = 2
} UART_StopBits;

/* Data Bits enumerator */
typedef enum {
  UART_DATA_BITS_5    = 5,
  UART_DATA_BITS_6    = 6,
  UART_DATA_BITS_7    = 7,
  UART_DATA_BITS_8    = 8
} UART_DataBits;

/* UART Port Properties structure */
typedef struct {
  uint32_t           Baudrate;
  UART_DataBits      DataBits;
  UART_Parity        Parity;
  UART_StopBits      StopBits;
} UART_Configuration;

void UART_RBR_Disable(char portNum);
void UART_RBR_Enable(char portNum);
void UART_Init(char portNum, UART_Configuration configuration);
void UART_SendChar(char portNum, char value);
void UART_SendBuffer(char portNum, char *buffer, unsigned int length);
char UART_GetChar(char portNum);
void UART_SetConfiguration(char portNum, UART_Configuration configuration);
extern void UART0_IRQHandler(void);
extern void UART1_IRQHandler(void);
extern void UART2_IRQHandler(void);
extern void UART3_IRQHandler(void);

#endif /* end __UART_H */
/***************************************************************************
                            End Of File
***************************************************************************/
