/****************************************************************************
	Luka Penger 2013
	UART
	LPC17xx
****************************************************************************/

#include "UART.h"

volatile unsigned int UART_Status[4];
volatile unsigned char UART_TxEmpty[4] = {1, 1, 1, 1};
volatile unsigned char UART_Buffer[4][UART_BUFSIZE];
volatile unsigned int UART_Count[4] = {0, 0, 0, 0};

void UART_RBR_Disable(char portNum) {
	switch (portNum) {
		case 0:
			LPC_UART0->IER = UART_IER_THRE | UART_IER_RLS;			/* Disable RBR */
			break;
		case 1:
			LPC_UART1->IER = UART_IER_THRE | UART_IER_RLS;			/* Disable RBR */
			break;
		case 2:
			LPC_UART2->IER = UART_IER_THRE | UART_IER_RLS;			/* Disable RBR */
			break;
		default:
			LPC_UART3->IER = UART_IER_THRE | UART_IER_RLS;			/* Disable RBR */
			break;
	}
}

void UART_RBR_Enable(char portNum) {
	switch (portNum) {
		case 0:
			LPC_UART0->IER = UART_IER_THRE | UART_IER_RLS | UART_IER_RBR;	/* Re-enable RBR */
			break;
		case 1:
			LPC_UART1->IER = UART_IER_THRE | UART_IER_RLS | UART_IER_RBR;	/* Re-enable RBR */
			break;
		case 2:
			LPC_UART2->IER = UART_IER_THRE | UART_IER_RLS | UART_IER_RBR;	/* Re-enable RBR */
			break;
		default:
			LPC_UART3->IER = UART_IER_THRE | UART_IER_RLS | UART_IER_RBR;	/* Re-enable RBR */
			break;
	}
}

void UART_Init(char portNum, UART_Configuration configuration) {
	switch (portNum) {
		case 0:

			break;
		case 1:
			LPC_SC->PCONP |= (1 << 4) | (1 << 15);/* Enable power to UART1 & IOCON      */

			LPC_PINCON->PINSEL4 |=  (2 << 0);     /* Pin P2.0 used as TXD1              */
			LPC_PINCON->PINSEL4 |=  (2 << 2);     /* Pin P2.1 used as RXD1              */

			LPC_UART1->IER = UART_IER_RBR | UART_IER_THRE | UART_IER_RLS;	/* Enable interrupt */

			NVIC_DisableIRQ (UART1_IRQn);         /* Disable UART interrupt             */

			LPC_UART1->FCR = 0x06;                /* Reset FIFOs                        */

			NVIC_EnableIRQ (UART1_IRQn);          /* Enable UART interrupt              */
			break;
		case 2:

			break;
		default:

			break;
	}
	
	UART_SetConfiguration(portNum, configuration);
}

void UART_SendChar(char portNum, char value) {
	if(portNum == 0)
  {
		/* THRE status, contain valid data */
		while(!(UART_TxEmpty[portNum] & 0x01) );	
		LPC_UART0->THR = value;
		UART_TxEmpty[portNum] = 0;	/* not empty in the THR until it shifts out */
  } else {
		/* THRE status, contain valid data */
		while(!(UART_TxEmpty[portNum] & 0x01) );	
		LPC_UART1->THR = value;
		UART_TxEmpty[portNum] = 0;	/* not empty in the THR until it shifts out */
  }
}

void UART_SendBuffer(char portNum, char *buffer, unsigned int length) {
	while (length != 0)
	{
		UART_SendChar(portNum, *buffer);
		buffer++;
		length--;
	}
}

char UART_GetChar(char portNum) {
	switch (portNum) {
		case 0:
			while (!(LPC_UART0->LSR & 0x01));
			return (LPC_UART0->RBR);
		case 1:
			while (!(LPC_UART1->LSR & 0x01));
			return (LPC_UART1->RBR);
		case 2:
			while (!(LPC_UART2->LSR & 0x01));
			return (LPC_UART2->RBR);
		default:
			while (!(LPC_UART3->LSR & 0x01));
			return (LPC_UART3->RBR);
	}
}

void UART_SetConfiguration(char portNum, UART_Configuration configuration) {
  uint32_t lcr = 0;
	uint32_t Dll;
	
  switch (configuration.DataBits) {           /* Prepare data bits value for MR reg */
    case UART_DATA_BITS_5:
      break;
    case UART_DATA_BITS_6:
      lcr |= 1;
      break;
    case UART_DATA_BITS_7:
      lcr |= 2;
      break;
    case UART_DATA_BITS_8:
      lcr |= 3;
      break;
  }

  switch (configuration.Parity) {             /* Prepare parity value for MR reg    */
    case UART_PARITY_NONE:
      break;
    case UART_PARITY_ODD:
      lcr |= (1 << 3) | (0 << 4);
      break;
    case UART_PARITY_EVEN:
      lcr |= (1 << 3) | (1 << 4);
      break;
    case UART_PARITY_MARK:
      lcr |= (1 << 3) | (2 << 4);
      break;
    case UART_PARITY_SPACE:
      lcr |= (1 << 3) | (3 << 4);
      break;
  }

  switch (configuration.StopBits) {           /* Prepare stop bits value for MR reg */
    case UART_STOP_BITS_1:
    case UART_STOP_BITS_1_5:
      break;
    case UART_STOP_BITS_2:
      lcr |= 1 << 2;
      break;
  }

  Dll = ((UART_CLK / configuration.Baudrate) / 16);

	switch (portNum) {
		case 1:
			LPC_UART1->LCR =  0x80;                    /* DLAB = 1                      */
			LPC_UART1->FDR =  0;                       /* Fractional div not used (==1) */
			LPC_UART1->DLL =  Dll;                     /* Low divisor latch             */
			LPC_UART1->DLM = (Dll >> 8);               /* High divisor latch            */
			LPC_UART1->LCR =  lcr;                     /* Line Control parameters       */
			break;
	}
}

/*------------------------------------------------------------------------------
 * UART_IRQ:        Serial interrupt handler routine
 *----------------------------------------------------------------------------*/

extern void UART0_IRQHandler(void) {
	unsigned char IIRValue, LSRValue;
  unsigned char Dummy = Dummy;
	
  IIRValue = LPC_UART0->IIR;
    
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
	
  if(IIRValue == UART_IIR_RLS)		/* Receive Line Status */
  {
		LSRValue = LPC_UART0->LSR;
		/* Receive Line Status */
		if(LSRValue & (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_RXFE | UART_LSR_BI))
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			UART_Status[0] = LSRValue;
			Dummy = LPC_UART0->RBR;		/* Dummy read on RX to clear 
								interrupt, then bail out */
			return;
		}
		
		if(LSRValue & UART_LSR_RDR)	/* Receive Data Ready */			
		{
			/* If no error on RLS, normal ready, save into the data buffer. */
			/* Note: read RBR will clear the interrupt */
			UART_Buffer[0][UART_Count[0]] = LPC_UART0->RBR;
			UART_Count[0]++;
			if (UART_Count[0] == UART_BUFSIZE)
			{
				UART_Count[0] = 0;		/* buffer overflow */
			}	
		}
  } else if(IIRValue == UART_IIR_RDA)	{	/* Receive Data Available */
		/* Receive Data Available */
		UART_Buffer[0][UART_Count[0]] = LPC_UART0->RBR;
		UART_Count[0]++;
		
		if(UART_Count[0] == UART_BUFSIZE)
		{
			UART_Count[0] = 0;		/* buffer overflow */
		}
	}	else if (IIRValue == UART_IIR_CTI)	{ /* Character timeout indicator */
		/* Character Time-out indicator */
		UART_Status[0] |= 0x100;		/* Bit 9 as the CTI error */
	} else if(IIRValue == UART_IIR_THRE) { /* THRE, transmit holding register empty */
		/* THRE interrupt */
		LSRValue = LPC_UART0->LSR;		/* Check status in the LSR to see if
										valid data in U0THR or not */
		if (LSRValue & UART_LSR_THRE)
		{
			UART_TxEmpty[0] = 1;
		}
		else
		{
			UART_TxEmpty[0] = 0;
		}
  }
}

extern void UART1_IRQHandler(void) {
	unsigned char IIRValue, LSRValue;
  unsigned char Dummy = Dummy;
	
  IIRValue = LPC_UART1->IIR;
    
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
	
  if(IIRValue == UART_IIR_RLS)		/* Receive Line Status */
  {
		LSRValue = LPC_UART1->LSR;
		/* Receive Line Status */
		if(LSRValue & (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_RXFE | UART_LSR_BI))
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			UART_Status[1] = LSRValue;
			Dummy = LPC_UART1->RBR;		/* Dummy read on RX to clear 
								interrupt, then bail out */
			return;
		}
		
		if(LSRValue & UART_LSR_RDR)	/* Receive Data Ready */			
		{
			/* If no error on RLS, normal ready, save into the data buffer. */
			/* Note: read RBR will clear the interrupt */
			UART_Buffer[1][UART_Count[1]] = LPC_UART1->RBR;
			UART_Count[1]++;
			if (UART_Count[1] == UART_BUFSIZE)
			{
				UART_Count[1] = 0;		/* buffer overflow */
			}	
		}
  } else if(IIRValue == UART_IIR_RDA)	{	/* Receive Data Available */
		/* Receive Data Available */
		UART_Buffer[1][UART_Count[1]] = LPC_UART1->RBR;
		UART_Count[1]++;
		
		if(UART_Count[1] == UART_BUFSIZE)
		{
			UART_Count[1] = 0;		/* buffer overflow */
		}
	}	else if (IIRValue == UART_IIR_CTI)	{ /* Character timeout indicator */
		/* Character Time-out indicator */
		UART_Status[1] |= 0x100;		/* Bit 9 as the CTI error */
	} else if(IIRValue == UART_IIR_THRE) { /* THRE, transmit holding register empty */
		/* THRE interrupt */
		LSRValue = LPC_UART1->LSR;		/* Check status in the LSR to see if
										valid data in U0THR or not */
		if (LSRValue & UART_LSR_THRE)
		{
			UART_TxEmpty[1] = 1;
		}
		else
		{
			UART_TxEmpty[1] = 0;
		}
  }
}

extern void UART2_IRQHandler(void) {
	unsigned char IIRValue, LSRValue;
  unsigned char Dummy = Dummy;
	
  IIRValue = LPC_UART2->IIR;
    
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
	
  if(IIRValue == UART_IIR_RLS)		/* Receive Line Status */
  {
		LSRValue = LPC_UART2->LSR;
		/* Receive Line Status */
		if(LSRValue & (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_RXFE | UART_LSR_BI))
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			UART_Status[2] = LSRValue;
			Dummy = LPC_UART2->RBR;		/* Dummy read on RX to clear 
								interrupt, then bail out */
			return;
		}
		
		if(LSRValue & UART_LSR_RDR)	/* Receive Data Ready */			
		{
			/* If no error on RLS, normal ready, save into the data buffer. */
			/* Note: read RBR will clear the interrupt */
			UART_Buffer[2][UART_Count[2]] = LPC_UART0->RBR;
			UART_Count[2]++;
			if (UART_Count[2] == UART_BUFSIZE)
			{
				UART_Count[2] = 0;		/* buffer overflow */
			}	
		}
  } else if(IIRValue == UART_IIR_RDA)	{	/* Receive Data Available */
		/* Receive Data Available */
		UART_Buffer[2][UART_Count[2]] = LPC_UART0->RBR;
		UART_Count[2]++;
		
		if(UART_Count[2] == UART_BUFSIZE)
		{
			UART_Count[2] = 0;		/* buffer overflow */
		}
	}	else if (IIRValue == UART_IIR_CTI)	{ /* Character timeout indicator */
		/* Character Time-out indicator */
		UART_Status[2] |= 0x100;		/* Bit 9 as the CTI error */
	} else if(IIRValue == UART_IIR_THRE) { /* THRE, transmit holding register empty */
		/* THRE interrupt */
		LSRValue = LPC_UART2->LSR;		/* Check status in the LSR to see if
										valid data in U0THR or not */
		if (LSRValue & UART_LSR_THRE)
		{
			UART_TxEmpty[2] = 1;
		}
		else
		{
			UART_TxEmpty[2] = 0;
		}
  }
}

extern void UART3_IRQHandler(void) {
	unsigned char IIRValue, LSRValue;
  unsigned char Dummy = Dummy;
	
  IIRValue = LPC_UART3->IIR;
    
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
	
  if(IIRValue == UART_IIR_RLS)		/* Receive Line Status */
  {
		LSRValue = LPC_UART3->LSR;
		/* Receive Line Status */
		if(LSRValue & (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_RXFE | UART_LSR_BI))
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			UART_Status[3] = LSRValue;
			Dummy = LPC_UART3->RBR;		/* Dummy read on RX to clear 
								interrupt, then bail out */
			return;
		}
		
		if(LSRValue & UART_LSR_RDR)	/* Receive Data Ready */			
		{
			/* If no error on RLS, normal ready, save into the data buffer. */
			/* Note: read RBR will clear the interrupt */
			UART_Buffer[3][UART_Count[3]] = LPC_UART3->RBR;
			UART_Count[3]++;
			if (UART_Count[3] == UART_BUFSIZE)
			{
				UART_Count[3] = 0;		/* buffer overflow */
			}	
		}
  } else if(IIRValue == UART_IIR_RDA)	{	/* Receive Data Available */
		/* Receive Data Available */
		UART_Buffer[3][UART_Count[3]] = LPC_UART3->RBR;
		UART_Count[3]++;
		
		if(UART_Count[3] == UART_BUFSIZE)
		{
			UART_Count[3] = 0;		/* buffer overflow */
		}
	}	else if (IIRValue == UART_IIR_CTI)	{ /* Character timeout indicator */
		/* Character Time-out indicator */
		UART_Status[3] |= 0x100;		/* Bit 9 as the CTI error */
	} else if(IIRValue == UART_IIR_THRE) { /* THRE, transmit holding register empty */
		/* THRE interrupt */
		LSRValue = LPC_UART3->LSR;		/* Check status in the LSR to see if
										valid data in U0THR or not */
		if (LSRValue & UART_LSR_THRE)
		{
			UART_TxEmpty[3] = 1;
		}
		else
		{
			UART_TxEmpty[3] = 0;
		}
  }
}

/****************************************************************************
                            End Of File
****************************************************************************/
