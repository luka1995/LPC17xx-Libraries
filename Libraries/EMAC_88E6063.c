/****************************************************************************
	Luka Penger 2013
	EMAC 88E6063 Marvell
	LPC1768
****************************************************************************/

#include <EMAC_88E6063.H>

/* The following macro definitions may be used to select the speed
   of the physical link:

  _10MBIT_   - connect at 10 MBit only
  _100MBIT_  - connect at 100 MBit only

  By default an autonegotiation of the link speed is used. This may take 
  longer to connect, but it works for 10MBit and 100MBit physical links.     */

/* Net_Config.c */
extern U8 own_hw_adr[];

/* EMAC local DMA Descriptors. */
static            RX_Desc Rx_Desc[NUM_RX_FRAG];
static __align(8) RX_Stat Rx_Stat[NUM_RX_FRAG]; /* Must be 8-Byte alligned   */
static            TX_Desc Tx_Desc[NUM_TX_FRAG];
static            TX_Stat Tx_Stat[NUM_TX_FRAG];

/* EMAC local DMA buffers. */
static U32 rx_buf[NUM_RX_FRAG][ETH_FRAG_SIZE>>2];
static U32 tx_buf[NUM_TX_FRAG][ETH_FRAG_SIZE>>2];

/*----------------------------------------------------------------------------
 *      EMAC Ethernet Driver Functions
 *----------------------------------------------------------------------------
 *  Required functions for Ethernet driver module:
 *  a. Polling mode: - void init_ethernet ()
 *                   - void send_frame (OS_FRAME *frame)
 *                   - void poll_ethernet (void)
 *  b. Interrupt mode: - void init_ethernet ()
 *                     - void send_frame (OS_FRAME *frame)
 *                     - void int_enable_eth ()
 *                     - void int_disable_eth ()
 *                     - interrupt function 
 *---------------------------------------------------------------------------*/

/* Local Function Prototypes */
static void rx_descr_init (void);
static void tx_descr_init (void);

/*--------------------------- init_ethernet ---------------------------------*/

void init_ethernet (void) {
  /* Initialize the EMAC ethernet controller. */
  U32 regv,tout;

  /* Power Up the EMAC controller. */
  LPC_SC->PCONP |= 0x40000000;

  /* Enable P1 Ethernet Pins. */
  LPC_PINCON->PINSEL2 = 0x50150105;
	
  /* LPC176x devices, no MDIO, MDC remap. */
  LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL3 & ~0x0000000F) | 0x00000005;

  /* Reset all EMAC internal modules. */
  LPC_EMAC->MAC1    = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | 
                      MAC1_RES_MCS_RX | MAC1_SIM_RES | MAC1_SOFT_RES;
  LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES | CR_PASS_RUNT_FRM;

  /* A short delay after reset. */
  for (tout = 100; tout; tout--);

  /* Initialize MAC control registers. */
  LPC_EMAC->MAC1 = MAC1_PASS_ALL;
  LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
  LPC_EMAC->MAXF = ETH_MAX_FLEN;
  LPC_EMAC->CLRT = CLRT_DEF;
  LPC_EMAC->IPGR = IPGR_DEF;

  /* Enable Reduced MII interface. */
  LPC_EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM;

  /* Reset Reduced MII Logic. */
  LPC_EMAC->SUPP = SUPP_RES_RMII;
  for (tout = 100; tout; tout--);
  LPC_EMAC->SUPP = 0;

  /* Put the MARVELL in reset mode */
  write_PHY (PHY_REG_CR, 0x8000);

  /* Wait for hardware reset to end. */
  for (tout = 0; tout < 0x100000; tout++) {
    regv = read_PHY (PHY_REG_CR);
    if (!(regv & 0x8000)) {
      /* Reset complete, device not Power Down. */
      break;
    }
  }
	
  /* Check the link status. */
	for (tout = 0; tout < 0x10000; tout++) {
		regv = read_PHY (PHY_REG_SSR);
		if (regv & 0x0400) {
			/* Link is on. */
			break;
		}
	}

  /* Configure Full/Half Duplex mode. */
	regv = read_PHY (PHY_REG_SSR);
  if (regv & 0x2000) {
    /* Full duplex is enabled. */
    LPC_EMAC->MAC2    |= MAC2_FULL_DUP;
    LPC_EMAC->Command |= CR_FULL_DUP;
    LPC_EMAC->IPGT     = IPGT_FULL_DUP;
  } else {
    /* Half duplex mode. */
    LPC_EMAC->IPGT = IPGT_HALF_DUP;
  }
	
	/* Configure 100MBit/10MBit mode. */
	regv = read_PHY (PHY_REG_SSR);
  if (regv & 0x4000) {
    /* 100MBit mode. */
    LPC_EMAC->SUPP = SUPP_SPEED;
  }
  else {
    /* 10MBit mode. */
    LPC_EMAC->SUPP = 0;
  }
	
/* Set the Ethernet MAC Address registers */
  LPC_EMAC->SA0 = ((U32)own_hw_adr[5] << 8) | (U32)own_hw_adr[4];
  LPC_EMAC->SA1 = ((U32)own_hw_adr[3] << 8) | (U32)own_hw_adr[2];
  LPC_EMAC->SA2 = ((U32)own_hw_adr[1] << 8) | (U32)own_hw_adr[0];

  /* Initialize Tx and Rx DMA Descriptors */
  rx_descr_init ();
  tx_descr_init ();

  /* Receive Broadcast, Multicast and Perfect Match Packets */
  LPC_EMAC->RxFilterCtrl = RFC_MCAST_EN | RFC_BCAST_EN | RFC_PERFECT_EN;

  /* Enable EMAC interrupts. */
  LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;

  /* Reset all interrupts */
  LPC_EMAC->IntClear  = 0xFFFF;

  /* Enable receive and transmit mode of MAC Ethernet core */
  LPC_EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
  LPC_EMAC->MAC1     |= MAC1_REC_EN;
}

/*--------------------------- int_enable_eth --------------------------------*/

void int_enable_eth (void) {
  /* Ethernet Interrupt Enable function. */
  NVIC_EnableIRQ(ENET_IRQn);
}

/*--------------------------- int_disable_eth -------------------------------*/

void int_disable_eth (void) {
  /* Ethernet Interrupt Disable function. */
  NVIC_DisableIRQ(ENET_IRQn);
}

/*--------------------------- send_frame ------------------------------------*/

void send_frame (OS_FRAME *frame) {
  /* Send frame to EMAC ethernet controller */
  U32 idx,len;
  U32 *sp,*dp;

  idx = LPC_EMAC->TxProduceIndex;
  sp  = (U32 *)&frame->data[0];
  dp  = (U32 *)Tx_Desc[idx].Packet;

  /* Copy frame data to EMAC packet buffers. */
  for (len = (frame->length + 3) >> 2; len; len--) {
    *dp++ = *sp++;
  }
  Tx_Desc[idx].Ctrl = (frame->length-1) | (TCTRL_INT | TCTRL_LAST);

  /* Start frame transmission. */
  if (++idx == NUM_TX_FRAG) idx = 0;
  LPC_EMAC->TxProduceIndex = idx;
}

/*--------------------------- interrupt_ethernet ----------------------------*/
  OS_FRAME *frame;
  U32 idx,int_stat,RxLen,info;
  U32 *sp,*dp;

void ENET_IRQHandler (void) {
  /* EMAC Ethernet Controller Interrupt function. */

  while ((int_stat = (LPC_EMAC->IntStatus & LPC_EMAC->IntEnable)) != 0) {
    LPC_EMAC->IntClear = int_stat;
    if (int_stat & INT_RX_DONE) {
      /* Packet received, check if packet is valid. */
			idx = LPC_EMAC->RxConsumeIndex;
			
    }
    if (int_stat & INT_TX_DONE) {
      /* Frame transmit completed. */
    }
  }
}

/*--------------------------- rx_descr_init ---------------------------------*/

static void rx_descr_init (void) {
  /* Initialize Receive Descriptor and Status array. */
  U32 i;

  for (i = 0; i < NUM_RX_FRAG; i++) {
    Rx_Desc[i].Packet  = (U32)&rx_buf[i];
    Rx_Desc[i].Ctrl    = RCTRL_INT | (ETH_FRAG_SIZE-1);
    Rx_Stat[i].Info    = 0;
    Rx_Stat[i].HashCRC = 0;
  }

  /* Set EMAC Receive Descriptor Registers. */
  LPC_EMAC->RxDescriptor       = (U32)&Rx_Desc[0];
  LPC_EMAC->RxStatus           = (U32)&Rx_Stat[0];
  LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

  /* Rx Descriptors Point to 0 */
  LPC_EMAC->RxConsumeIndex  = 0;
}


/*--------------------------- tx_descr_init ---- ----------------------------*/

static void tx_descr_init (void) {
  /* Initialize Transmit Descriptor and Status array. */
  U32 i;

  for (i = 0; i < NUM_TX_FRAG; i++) {
    Tx_Desc[i].Packet = (U32)&tx_buf[i];
    Tx_Desc[i].Ctrl   = 0;
    Tx_Stat[i].Info   = 0;
  }

  /* Set EMAC Transmit Descriptor Registers. */
  LPC_EMAC->TxDescriptor       = (U32)&Tx_Desc[0];
  LPC_EMAC->TxStatus           = (U32)&Tx_Stat[0];
  LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

  /* Tx Descriptors Point to 0 */
  LPC_EMAC->TxProduceIndex  = 0;
}

/*--------------------------- output_MDIO -----------------------------------*/

#define delay()     __nop(); __nop(); __nop();

static void output_MDIO (U32 val, U32 n) {
  /* Output a value to the MII PHY management interface. */

  for (val <<= (32 - n); n; val <<= 1, n--) {
    if (val & 0x80000000) {
      LPC_GPIO2->FIOSET = MDIO;
    }
    else {
      LPC_GPIO2->FIOCLR = MDIO;
    }
    delay ();
    LPC_GPIO2->FIOSET = MDC;
    delay ();
    LPC_GPIO2->FIOCLR = MDC;
  }
}

/*--------------------------- turnaround_MDIO -------------------------------*/

static void turnaround_MDIO (void) {
  /* Turnaround MDO is tristated. */

  LPC_GPIO2->FIODIR &= ~MDIO;
  LPC_GPIO2->FIOSET  = MDC;
  delay ();
  LPC_GPIO2->FIOCLR  = MDC;
  delay ();
}

/*--------------------------- input_MDIO ------------------------------------*/

static U32 input_MDIO (void) {
  /* Input a value from the MII PHY management interface. */
  U32 i,val = 0;

  for (i = 0; i < 16; i++) {
    val <<= 1;
    LPC_GPIO2->FIOSET = MDC;
    delay ();
    LPC_GPIO2->FIOCLR = MDC;
    if (LPC_GPIO2->FIOPIN & MDIO) {
      val |= 1;
    }
  }
  return (val);
}

/*--------------------------- write_PHY -------------------------------------*/

void write_PHY (U32 PhyReg, U16 Value) {
  /* Write a data 'Value' to PHY register 'PhyReg'. */

	/* Software MII Management for LPC175x. */
	/* Remapped MDC on P2.8 and MDIO on P2.9 do not work. */
	LPC_GPIO2->FIODIR |= MDIO;

	/* 32 consecutive ones on MDO to establish sync */
	output_MDIO (0xFFFFFFFF, 32);

	/* start code (01), write command (01) */
	output_MDIO (0x05, 4);

	/* write PHY address */
	output_MDIO (DEVICE_DEF_ADR >> 8, 5);

	/* write the PHY register to write */
	output_MDIO (PhyReg, 5);

	/* turnaround MDIO (1,0)*/
	output_MDIO (0x02, 2);

	/* write the data value */
	output_MDIO (Value, 16);

	/* turnaround MDO is tristated */
	turnaround_MDIO ();
}

/*--------------------------- read_PHY --------------------------------------*/

U16 read_PHY (U32 PhyReg) {
  /* Read a PHY register 'PhyReg'. */
  U32 val;

	/* Software MII Management for LPC175x. */
	/* Remapped MDC on P2.8 and MDIO on P2.9 does not work. */
	LPC_GPIO2->FIODIR |= MDIO;

	/* 32 consecutive ones on MDO to establish sync */
	output_MDIO (0xFFFFFFFF, 32);

	/* start code (01), read command (10) */
	output_MDIO (0x06, 4);

	/* write PHY address */
	output_MDIO (DEVICE_DEF_ADR >> 8, 5);

	/* write the PHY register to write */
	output_MDIO (PhyReg, 5);

	/* turnaround MDO is tristated */
	turnaround_MDIO ();

	/* read the data value */
	val = input_MDIO ();

	/* turnaround MDIO is tristated */
	turnaround_MDIO ();

  return (val);
}

/****************************************************************************
                            End Of File
****************************************************************************/
