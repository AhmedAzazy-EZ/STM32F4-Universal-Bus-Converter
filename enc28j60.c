#include <stm32f4xx_hal.h>
#include "enc28j60.h"
#include <stdint.h>
#include <stdbool.h>

//uint16_t bufferSize;
bool ENC28J60_broadcast_enabled = false;
bool ENC28J60_promiscuous_enabled = false;

static uint8_t waitgwmac; // Bitwise flags of gateway router status - see below for states
//Define gateway router ARP statuses
#define WGW_INITIAL_ARP 1      // First request, no answer yet
#define WGW_HAVE_GW_MAC 2      // Have gateway router MAC
#define WGW_REFRESHING 4       // Refreshing but already have gateway MAC
#define WGW_ACCEPT_ARP_REPLY 8 // Accept an ARP reply

uint16_t delaycnt = 0; //request gateway ARP lookup

/* 
	in the C++ land of arduino buffer[] resolves through defines and pointers
	to the following allocation gPB which is defined in dchp.cpp
*/
//uint8_t gPB[500]; // also called buffer through a define directive

//variables


SPI_HandleTypeDef hspi1; //dummy for now
#define hspi				hspi1
#define PORT_CS			GPIOA
#define PIN_CS			GPIO_PIN_4

extern SPI_HandleTypeDef hspi1;
static uint8_t Enc28_Bank;

void enableChip()
{
	HAL_GPIO_WritePin(PORT_CS,PIN_CS,GPIO_PIN_RESET);
}

void disableChip()
{
	HAL_GPIO_WritePin(PORT_CS,PIN_CS,GPIO_PIN_SET);
}

uint8_t ENC28_readOp(uint8_t oper, uint8_t addr)
{
   uint8_t spiData[2];
   enableChip();
   spiData[0] = (oper | (addr & ADDR_MASK));
   HAL_SPI_Transmit(&hspi, spiData, 1, 100);
   if (addr & 0x80)
   {
      //HAL_SPI_Transmit(&hspi, spiData, 1, 100);
      HAL_SPI_Receive(&hspi, &spiData[1], 1, 100);
   }
   HAL_SPI_Receive(&hspi, &spiData[1], 1, 100);
   disableChip();

   return spiData[1];
}
void ENC28_writeOp(uint8_t oper, uint8_t addr, uint8_t data)
{
   uint8_t spiData[2];
   enableChip();
   spiData[0] = (oper | (addr & ADDR_MASK)); //((oper<<5)&0xE0)|(addr & ADDR_MASK);
   spiData[1] = data;
   HAL_SPI_Transmit(&hspi, spiData, 2, 100);
   disableChip();
}
uint8_t ENC28_readReg8(uint8_t addr)
{
   ENC28_setBank(addr);
   return ENC28_readOp(ENC28J60_READ_CTRL_REG, addr);
}

void ENC28_writeReg8(uint8_t addr, uint8_t data)
{
   ENC28_setBank(addr);
   ENC28_writeOp(ENC28J60_WRITE_CTRL_REG, addr, data);
}

uint16_t ENC28_readReg16(uint8_t addr)
{
   return ENC28_readReg8(addr) + (ENC28_readReg8(addr + 1) << 8);
}

void ENC28_writeReg16(uint8_t addrL, uint16_t data)
{
   ENC28_writeReg8(addrL, data);
   ENC28_writeReg8(addrL + 1, data >> 8);
}

void ENC28_setBank(uint8_t addr)
{
   if ((addr & BANK_MASK) != Enc28_Bank)
   {
      ENC28_writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1 | ECON1_BSEL0);
      Enc28_Bank = addr & BANK_MASK;
      ENC28_writeOp(ENC28J60_BIT_FIELD_SET, ECON1, Enc28_Bank >> 5);
   }
}

void ENC28_writePhy(uint8_t addr, uint16_t data)
{
   ENC28_writeReg8(MIREGADR, addr);
   ENC28_writeReg16(MIWR, data);
   while (ENC28_readReg8(MISTAT) & MISTAT_BUSY)
      ;
}

uint8_t ENC28_readPhyByte(uint8_t addr)
{
   ENC28_writeReg8(MIREGADR, addr);     // pass the PHY address to the MII
   ENC28_writeReg8(MICMD, MICMD_MIIRD); // Enable Read bit
   while (ENC28_readReg8(MISTAT) & MISTAT_BUSY)
      ;                          // Poll for end of reading
   ENC28_writeReg8(MICMD, 0x00); // Disable MII Read

   //	return ENC28_readReg8(MIRDL) | (ENC28_readReg8(MIRDH) << 8);
   return ENC28_readReg8(MIRDH);
}

uint8_t ENC28_Init(const uint8_t *macaddr)
{
   /* in Arduino lib this function is called with a buffer size, mac address and CS pin
	 * this buffer does not seem to be used anywhere local
	uint8_t spiData[2];

	*/
   //bufferSize = sizeof(gPB); // sets up read buffer sise

   // (1): Disable the chip CS pin
   disableChip();
   HAL_Delay(1);

   // (2): Perform soft reset to the ENC28J60 module
   ENC28_writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
   HAL_Delay(2);

   // (3): Wait untill Clock is ready
   while (!ENC28_readOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY)
      ;

   // (4): Initialise RX and TX buffer size
   ENC28_writeReg16(ERXST, RXSTART_INIT);
   ENC28_writeReg16(ERXRDPT, RXSTART_INIT);
   ENC28_writeReg16(ERXND, RXSTOP_INIT);
   ENC28_writeReg16(ETXST, TXSTART_INIT);
   ENC28_writeReg16(ETXND, TXSTOP_INIT);

   // Arduino lib set this here
   // Stretch pulses for LED, LED_A=Link, LED_B=activity
   ENC28_writePhy(PHLCON, 0x476);

   // (5): Receive buffer filters
   ENC28_writeReg8(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN | ERXFCON_BCEN);

   // additional Arduino setup
   ENC28_writeReg16(EPMM0, 0x303f); // pattern match filter
   ENC28_writeReg16(EPMCS, 0xf7f9); // pattern match checksum filter

   // (6): MAC Control Register 1
   //	ENC28_writeReg8(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS|MACON1_PASSALL);
   // changed to
   ENC28_writeReg8(MACON1, MACON1_MARXEN);

   // (7): MAC Control Register 3
   ENC28_writeOp(ENC28J60_BIT_FIELD_SET, MACON3,
                 MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);

   // (8): NON/Back to back gap
   ENC28_writeReg16(MAIPG, 0x0C12); // NonBackToBack gap
   ENC28_writeReg8(MABBIPG, 0x12);  // BackToBack gap

   // (9): Set Maximum framelenght
   ENC28_writeReg16(MAMXFL, MAX_FRAMELEN); // Set Maximum frame length (any packet bigger will be discarded)

   // (10): Set the MAC address of the device
   ENC28_writeReg8(MAADR5, macaddr[0]);
   ENC28_writeReg8(MAADR4, macaddr[1]);
   ENC28_writeReg8(MAADR3, macaddr[2]);
   ENC28_writeReg8(MAADR2, macaddr[3]);
   ENC28_writeReg8(MAADR1, macaddr[4]);
   ENC28_writeReg8(MAADR0, macaddr[5]);

   /* 
		could back check the MADDR registers and see if they are
		loaded with the correct MAC
	*/

   //**********Advanced Initialisations************//
   // (1): Initialise PHY layer registers
   //	ENC28_writePhy(PHLCON, PHLCON_LED);
   ENC28_writePhy(PHCON2, PHCON2_HDLDIS);

   // (2): Enable Rx interrupt line
   ENC28_setBank(ECON1);

   ENC28_writeOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
   ENC28_writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

   // not used in Arduino code
   // ENC28_writeOp(ENC28J60_BIT_FIELD_SET, EIR, EIR_PKTIF);

   uint8_t rev = ENC28_readReg8(EREVID);
   // microchip forgot to step the number on the silicon when they
   // released the revision B7. 6 is now rev B7. We still have
   // to see what they do when they release B8. At the moment
   // there is no B8 out yet
   if (rev > 5)
      ++rev; // implement arduino's revision value return.
   return rev;
}

bool ENC28J60_isLinkUp(void)
{
   return (ENC28_readPhyByte(PHSTAT2) >> 2) & 1;
}

void ENC28J60_enableBroadcast(bool temporary)
{
   ENC28_writeReg8(ERXFCON, ENC28_readReg8(ERXFCON) | ERXFCON_BCEN);
   if (!temporary)
      ENC28J60_broadcast_enabled = true;
}

void ENC28J60_disableBroadcast(bool temporary)
{
   if (!temporary)
      ENC28J60_broadcast_enabled = false;
   if (!ENC28J60_broadcast_enabled)
      ENC28_writeReg8(ERXFCON, ENC28_readReg8(ERXFCON) & ~ERXFCON_BCEN);
}

#if 0

#else
static void ENC28_writeBuf(uint8_t *data, uint16_t len)
{
   uint8_t spiData[2];
   // enable chip
   enableChip();

   spiData[0] = ENC28J60_WRITE_BUF_MEM;
   HAL_SPI_Transmit(&hspi, spiData, 1, 100);

   //	spiData[1] = 0xFF;
   //	HAL_SPI_Transmit(&hspi, &spiData[1], 1, 100);

   HAL_SPI_Transmit(&hspi, data, len, 100);

   // disable chip
   disableChip();
}


void ENC28_packetSend(uint8_t *buf, uint16_t len)
{
   uint8_t retry = 0;

   while (1)
   {
      // latest errata sheet: DS80349C
      // always reset transmit logic (Errata Issue 12)
      // the Microchip TCP/IP stack implementation used to first check
      // whether TXERIF is set and only then reset the transmit logic
      // but this has been changed in later versions; possibly they
      // have a reason for this; they don't mention this in the errata
      // sheet
      ENC28_writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
      ENC28_writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
      ENC28_writeOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXERIF | EIR_TXIF);

      // prepare new transmission
      if (retry == 0)
      {
         ENC28_writeReg16(EWRPT, TXSTART_INIT);
         ENC28_writeReg16(ETXND, TXSTART_INIT + len);
         ENC28_writeOp(ENC28J60_WRITE_BUF_MEM, 0, 0); //line 485 enc28j60.cpp
         ENC28_writeBuf(buf, len);
      }

      // initiate transmission
      ENC28_writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

      // wait until transmission has finished; referring to the data sheet and
      // to the errata (Errata Issue 13; Example 1) you only need to wait until either
      // TXIF or TXERIF gets set; however this leads to hangs; apparently Microchip
      // realized this and in later implementations of their tcp/ip stack they introduced
      // a counter to avoid hangs; of course they didn't update the errata sheet
      uint16_t count = 0;
      while ((ENC28_readReg8(EIR) & (EIR_TXIF | EIR_TXERIF)) == 0 && ++count < 1000U)
         ;
      if (!(ENC28_readReg8(EIR) & EIR_TXERIF) && count < 1000U)
      {
         // no error; start new transmission
         break;
      }

      // cancel previous transmission if stuck
      ENC28_writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
      break;

      //retry++; // from Arduino enc28j60.cpp
   }
}

void readBuf(uint8_t *data, uint16_t len)
{
   //   uint8_t nextbyte;
   uint8_t spiData[2];

   enableChip();
   if (len != 0)
   {
      spiData[0] = ENC28J60_READ_BUF_MEM;
      HAL_SPI_Transmit(&hspi, spiData, 1, 100);
      HAL_SPI_Receive(&hspi, data, len, 100);
   }
   disableChip();
}

uint16_t ENC28J60_packetReceive(uint8_t *buf, int max_len)
{
   static uint16_t gNextPacketPtr = RXSTART_INIT;
   static bool unreleasedPacket = false;
   uint16_t len = 0;

   if (unreleasedPacket)
   {
      if (gNextPacketPtr == 0)
         ENC28_writeReg16(ERXRDPT, RXSTOP_INIT);
      else
         ENC28_writeReg16(ERXRDPT, gNextPacketPtr - 1);
      unreleasedPacket = false;
   }

   if (ENC28_readReg8(EPKTCNT) > 0)
   {
      ENC28_writeReg16(ERDPT, gNextPacketPtr);

      struct
      {
         uint16_t nextPacket;
         uint16_t byteCount;
         uint16_t status;
      } header;

      readBuf((uint8_t *)&header, sizeof header);

      gNextPacketPtr = header.nextPacket;
      len = header.byteCount - 4; //remove the CRC count
      if (len > max_len - 1)
         len = max_len - 1;
      if ((header.status & 0x80) == 0)
         len = 0;
      else
         readBuf(buf, len);
      buf[len] = 0;
      unreleasedPacket = true;

      ENC28_writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
   }
   return len;
}

#endif


