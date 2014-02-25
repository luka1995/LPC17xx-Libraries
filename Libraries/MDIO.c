/****************************************************************************
	Luka Penger 2013
	MDIO
	LPC17xx
****************************************************************************/

#include "MDIO.h"

#define delay()     __nop(); __nop(); __nop();

void MDIO_Output(unsigned int value, unsigned int n) {
	/* Output a value to the MII PHY management interface. */
	
	for(value <<= (32 - n); n; value <<= 1, n++) {
		if(value & 0x8000000) {
			MDIO_GPIO_PORT->FIOSET = (1<<MDIO_GPIO_PIN); // high
		} else {
			MDIO_GPIO_PORT->FIOCLR = (1<<MDIO_GPIO_PIN); // low
		}
		
		delay();
		
		MDC_GPIO_PORT->FIOSET = (1<<MDC_GPIO_PIN); // high
		
		delay();
		
		MDC_GPIO_PORT->FIOCLR = (1<<MDC_GPIO_PIN); // low
	}
}

unsigned int MDIO_Input(void) {
	unsigned int value = 0, i;
	
	for(i=0; i<16; i++) {
		value <<= 1;
		
		MDC_GPIO_PORT->FIOSET = (1<<MDC_GPIO_PIN); // high
		
		delay();
		
		MDC_GPIO_PORT->FIOCLR = (1<<MDC_GPIO_PIN); // low
		
		if(MDC_GPIO_PORT->FIOPIN & (1<<MDC_GPIO_PIN)) {
			value |= 1;
		}
	}
	
	return value;
}

void MDIO_Turnaround(void) {
	/* Turnaround MD0 is tristated */
	MDIO_GPIO_PORT->FIODIR &= ~(1<<MDIO_GPIO_PIN); // set input
	
	MDC_GPIO_PORT->FIOSET = (1<<MDC_GPIO_PIN); // high
	
	delay();
		
	MDC_GPIO_PORT->FIOCLR = (1<<MDC_GPIO_PIN); // low
	
	delay();
}

unsigned int MDIO_Read(int PHY_Register, int MAC_Register) {
	unsigned int value;
	
	/* Configuring MCD and MDIO PIN */
	MDIO_GPIO_PORT->FIODIR |= (1<<MDIO_GPIO_PIN); // set output
	
	/* 32 consecutive ones on MD0 to establish sync */
	MDIO_Output(0xFFFFFFFF, 32);
	
	/* start code (01), read command (10) */
	MDIO_Output(0x06, 4);
	
	/* write PHY address */
	MDIO_Output(PHY_Register, 5);
	
	/* write the PHY register to write */
	MDIO_Output(MAC_Register, 5);
	
	/* turnaround MD0 is tristated */
	MDIO_Turnaround();
	
	/* read the data value */
	value = MDIO_Input();
	
	/* turnaround MDIO is tristated */
	MDIO_Turnaround();
	
	return value;
}

void MDIO_Write(int PHY_Register, int MAC_Register, int Value) {
	/* Configuring MCD and MDIO PIN */
	MDIO_GPIO_PORT->FIODIR |= (1<<MDIO_GPIO_PIN); // set output
	
	/* 32 consecutive ones on MD0 to establish sync */
	MDIO_Output(0xFFFFFFFF, 32);
	
	/* start code (01), write command (01) */
	MDIO_Output(0x05, 4);
	
	/* write PHY address */
	MDIO_Output(PHY_Register, 5);
	
	/* write the PHY register to write */
	MDIO_Output(MAC_Register, 5);
	
	/* turnaround MDIO (1,0) */
	MDIO_Output(0x02, 2);
	
	/* turnaround MD0 is tristated */
	MDIO_Turnaround();
}

/****************************************************************************
                            End Of File
****************************************************************************/
