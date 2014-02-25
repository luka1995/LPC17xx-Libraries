/****************************************************************************
	Luka Penger 2013
	MDIO
	LPC17xx
****************************************************************************/
#ifndef __MDIO_H 
#define __MDIO_H

#include <LPC17xx.H>

#define MDIO_GPIO_PIN				9
#define MDC_GPIO_PIN				8

#define MDIO_GPIO_PORT              LPC_GPIO2
#define MDC_GPIO_PORT				LPC_GPIO2

void MDIO_Output(unsigned int value, unsigned int n);
unsigned int MDIO_Input(void);
void MDIO_Turnaround(void);
unsigned int MDIO_Read(int PHY_Register, int MAC_Register);
void MDIO_Write(int PHY_Register, int MAC_Register, int Value);

#endif /* end __MDIO_H */
/****************************************************************************
                            End Of File
****************************************************************************/
