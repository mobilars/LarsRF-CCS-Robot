/*
 * proprietary.c
 *
 *  Created on: 14. okt. 2013
 *      Author: Lars
 */

// *************************************************************************************************
// cSLight
// Based on Extension to drive the Conrad "Funk-Lichtschalter-mit-Sender"
// http://www.conrad.at/ce/de/product/620587/Funk-Lichtschalter-mit-Sender
// (c) Christian M. Schmid
// 1.1.2011
// adapted for Chronos working with AIR boosterpack by Cor Berrevoets
// *************************************************************************************************
// *************************************************************************************************
// Include section
// system
#include "project.h"
#include <string.h>

// driver
#include "ports.h"
#include "display.h"
#include "timer.h"
#include "rf1a.h"

// logic
#include "menu.h"
#include "clock.h"
//#include "cSLight.h"

#include "proprietary.h"

// *************************************************************************************************
// Prototypes section

void cSL_transmit_code(u8 x, u8 y, u8 z);
//void sx_cSLight(u8 line);
//extern void mx_cSLight(u8 line);
//void display_cSLight(u8 line, u8 update);
void InitRadio(void);
void WriteBurstPATable(unsigned char *buffer, unsigned char count);
void WriteRfSettings(RF_SETTINGS *pRfSettings);

// *************************************************************************************************
// Global Variable section
//volatile u8 cSLightConfig;

// *************************************************************************************************
// Define section
#define st(x)      					do { x } while (__LINE__ == -1)
#define ENTER_CRITICAL_SECTION(x)  	st( x = __get_interrupt_state(); __disable_interrupt(); )
#define EXIT_CRITICAL_SECTION(x)    __set_interrupt_state(x)

unsigned char cSLpaTable[2] = { 0xC3, 0xC3 }; //0 dBm

typedef volatile struct StatusStruct {
	enum TxACKMessageType {                           //!< Flags to ACK transfer
		REQUEST_NO_ACK = 0, REQUEST_ACK = 1
	} TxACKMessageType;

	enum RxACKMessageType {                           //!< Flags to ACK transfer
		RECEIVED_NO_ACK = 0, RECEIVED_ACK = 1
	} RxACKMessageType;

	char channel;
	char power;
	char _RSSI;
	unsigned int msgNodeId;
} StatusStruct;

typedef struct PayloadFrame {

	char addr; // payloads need to start with char addr as the receiver checks this !
	char msg; // messagetype 0x00 - 0xFF options
	char msgdata0; // var1 that belongs to the message
	char msgdata1; // var2 that belongs to the message
	char msgdata2; // var2 that belongs to the message

	StatusStruct Status;

	// USER DESIGNED PAYLOAD SECTION
	unsigned int data1;
	//unsigned int data2;
	char DataUnit;

} PayloadFrame;

PayloadFrame Out_Payload;  // receiving payload

#define maxPayload 40 // reserve space for the payloadsize in the buffers
unsigned char txBuffer[maxPayload];

// Rf settings for CC430

RF_SETTINGS cSLrfSettings = { 0x29, // IOCFG2           GDO2 Output Configuration
		0x2E,  // IOCFG1           GDO1 Output Configuration
		0x06,  // IOCFG0           GDO0 Output Configuration
		0x07,  // FIFOTHR          RX FIFO and TX FIFO Thresholds
		0xD3,  // SYNC1            Sync Word, High Byte
		0x91,  // SYNC0            Sync Word, Low Byte
		0xFF,  // PKTLEN           Packet Length
		0x04,  // PKTCTRL1         Packet Automation Control no adress check
		0x05,  // PKTCTRL0         Packet Automation Control
		0x01,  // ADDR             Device Address
		0x10,  // CHANNR           Channel Number
		0x0C,  // FSCTRL1          Frequency Synthesizer Control
		0x00,  // FSCTRL0          Frequency Synthesizer Control
		0x21,  // FREQ2            Frequency Control Word, High Byte
		0x62,  // FREQ1            Frequency Control Word, Middle Byte
		0x76,  // FREQ0            Frequency Control Word, Low Byte
		0x2D,  // MDMCFG4          Modem Configuration
		0x3B,  // MDMCFG3          Modem Configuration
		0x73, // MDMCFG2          Modem Configuration CAN BE 13 !! 2GFSK or 73 MSK
		0x22,  // MDMCFG1          Modem Configuration
		0xF8,  // MDMCFG0          Modem Configuration
		0x62,  // DEVIATN          Modem Deviation Setting
		0x07, // MCSM2            Main Radio Control State Machine Configuration
		0x30, // MCSM1            Main Radio Control State Machine Configuration
		0x10, // MCSM0            Main Radio Control State Machine Configuration
		0x1D,  // FOCCFG           Frequency Offset Compensation Configuration
		0x1C,  // BSCFG            Bit Synchronization Configuration
		0xC7,  // AGCCTRL2         AGC Control
		0x00,  // AGCCTRL1         AGC Control
		0xB0,  // AGCCTRL0         AGC Control
		0x87,  // WOREVT1          High Byte Event0 Timeout
		0x00,  // WOREVT0          Low Byte Event0 Timeout
		0xFB,  // WORCTRL          Wake On Radio Control
		0xB6,  // FREND1           Front End RX Configuration
		0x10,  // FREND0           Front End TX Configuration
		0xEA,  // FSCAL3           Frequency Synthesizer Calibration
		0x2A,  // FSCAL2           Frequency Synthesizer Calibration
		0x00,  // FSCAL1           Frequency Synthesizer Calibration
		0x1F,  // FSCAL0           Frequency Synthesizer Calibration
		0x59,  // FSTEST           Frequency Synthesizer Calibration Control
		0x7F,  // PTEST            Production Test
		0x3C,  // AGCTEST          AGC Test

		};
//
//RF_SETTINGS cSLrfSettings = { 0x29, // IOCFG2           GDO2 Output Configuration
//		0x2E,  // IOCFG1           GDO1 Output Configuration
//		0x06,  // IOCFG0           GDO0 Output Configuration
//		0x07,  // FIFOTHR          RX FIFO and TX FIFO Thresholds
//		0xD3,  // SYNC1            Sync Word, High Byte
//		0x91,  // SYNC0            Sync Word, Low Byte
//		0xFF,  // PKTLEN           Packet Length
//		0x67,  // PKTCTRL1         Packet Automation Control no adress check
//		0x45,  // PKTCTRL0         Packet Automation Control
//		0x01,  // ADDR             Device Address
//		0x00,  // CHANNR           Channel Number
//		0x06,  // FSCTRL1          Frequency Synthesizer Control
//		0x00,  // FSCTRL0          Frequency Synthesizer Control
//		0x21,  // FREQ2            Frequency Control Word, High Byte
//		0x62,  // FREQ1            Frequency Control Word, Middle Byte
//		0xED,  // FREQ0            Frequency Control Word, Low Byte
//		0xEA,  // MDMCFG4          Modem Configuration
//		0x71,  // MDMCFG3          Modem Configuration
//		0x73, // MDMCFG2          Modem Configuration CAN BE 13 !! 2GFSK or 73 MSK
//		0x20,  // MDMCFG1          Modem Configuration
//		0xF8,  // MDMCFG0          Modem Configuration
//		0x33,  // DEVIATN          Modem Deviation Setting
//		0x07, // MCSM2            Main Radio Control State Machine Configuration
//		0x30, // MCSM1            Main Radio Control State Machine Configuration
//		0x18, // MCSM0            Main Radio Control State Machine Configuration
//		0x16,  // FOCCFG           Frequency Offset Compensation Configuration
//		0x6C,  // BSCFG            Bit Synchronization Configuration
//		0xC7,  // AGCCTRL2         AGC Control
//		0x00,  // AGCCTRL1         AGC Control
//		0x91,  // AGCCTRL0         AGC Control
//		0x87,  // WOREVT1          High Byte Event0 Timeout
//		0x00,  // WOREVT0          Low Byte Event0 Timeout
//		0xFB,  // WORCTRL          Wake On Radio Control
//		0x56,  // FREND1           Front End RX Configuration
//		0x10,  // FREND0           Front End TX Configuration
//		0xEA,  // FSCAL3           Frequency Synthesizer Calibration
//		0x00,  // FSCAL1           Frequency Synthesizer Calibration
//		0x2A,  // FSCAL2           Frequency Synthesizer Calibration
//		0x1F,  // FSCAL0           Frequency Synthesizer Calibration
//		0x59,  // FSTEST           Frequency Synthesizer Calibration Control
//		0x7F,  // PTEST            Production Test
//		0x3C,  // AGCTEST          AGC Test
//
//		};

// *************************************************************************************************
// Global Variable section

// *************************************************************************************************
// Extern section

int radio_init = 0;
void init_proprietary() {
	if (radio_init == 0) {
		//Reset radio
		ResetRadioCore();
		InitRadio();
		display_symbol(LCD_ICON_BEEPER1, SEG_ON);
		display_symbol(LCD_ICON_BEEPER2, SEG_ON);
		display_symbol(LCD_ICON_BEEPER3, SEG_ON);
		radio_init = 1;
	} else {
		ResetRadioCore();
		InitRadio();
		display_symbol(LCD_ICON_BEEPER1, SEG_OFF);
		display_symbol(LCD_ICON_BEEPER2, SEG_OFF);
		display_symbol(LCD_ICON_BEEPER3, SEG_OFF);
		radio_init = 0;
		Strobe(RF_SPWD);
	}
}

void transmit_proprietary(u8 x, u8 y, u8 z) {

	if (radio_init == 0) {
			//Reset radio
			ResetRadioCore();
			InitRadio();
			display_symbol(LCD_ICON_BEEPER1, SEG_ON);
			display_symbol(LCD_ICON_BEEPER2, SEG_ON);
			display_symbol(LCD_ICON_BEEPER3, SEG_ON);
			radio_init = 1;
		}

	RF1AIES |= BIT9;
	RF1AIFG &= ~BIT9;                         // Clear pending interrupts
	RF1AIE &= ~BIT9;                       // Disable TX end-of-packet interrupt

	//Strobe(RF_SIDLE);
	//__delay_cycles(50000);
	cSL_transmit_code(x, y, z);
	//__delay_cycles(50000);
	Strobe(RF_SIDLE);
	Strobe(RF_SPWD);

	//Clear RF symbol
}

//void display_cSLight(u8 line, u8 update)
//{
//		display_chars(LCD_SEG_L1_3_0, (u8 *)"SEND", SEG_ON);
//}

//// transfer of a payload
//PayloadFrame * MakeTxPayload (u8 x, u8 y, u8 z)
//{	Out_Payload.Status.msgNodeId=1; //
//    Out_Payload.addr=0x01;
//    Out_Payload.msg=0x02; // MESSAGETYPE IS TIME MESSAGE
//    Out_Payload.msgdata0=x;
//    Out_Payload.msgdata1=y;
//    Out_Payload.msgdata2=z;
//    Out_Payload.data1=sTime.second;
//	return &Out_Payload;
//}
//
//
//// load a payload into the TxBuffer
//char PreparetxBuffer (char *payload, char len)
//{
//int i = 0;
//   for (i = len; i >= 0; i--)
//         {   txBuffer[i+1] = payload[i];                  // Create DataPacket from Payload
//         }
//   txBuffer[0]=len; 	// byte 0 in the pack should be the package length
//  return 0;
//}
//
//char txBuffer[12];

void cSL_transmit_code(u8 x, u8 y, u8 z) {
	//PreparetxBuffer( (char *)(MakeTxPayload(x,y,z)), sizeof(PayloadFrame));

	txBuffer[0] = 11;                        // Packet length
	txBuffer[1] = 0x01;                     // Packet address
	txBuffer[2] = 0x00;
	txBuffer[3] = x;
	txBuffer[4] = y;
	txBuffer[5] = z;
	txBuffer[6] = 0x35;
	txBuffer[7] = 0x36;
	txBuffer[8] = 0x37;
	txBuffer[9] = 0x38;
	txBuffer[10] = 0x39;
	txBuffer[11] = 0x40;
	//PreparetxBuffer( (char *)(MakeTxPayload(u8 x, u8 y, u8 z)), sizeof(PayloadFrame));
	//WriteBurstReg(RF_TXFIFOWR, txBuffer, txBuffer[0]+1);

	Strobe(RF_SIDLE);
	Strobe(RF_SFRX);
	Strobe(RF_SFTX);

	WriteBurstReg(RF_TXFIFOWR, txBuffer, 12); // Write TX packet structure data

	//WriteBurstReg(RF_TXFIFOWR, cSLcommand, 19);

	Strobe(RF_STX);

	Timer0_A4_Delay(CONV_MS_TO_TICKS(42));
	//Timer0_A4_Delay(CONV_MS_TO_TICKS(1000));
	while ((Strobe(RF_SNOP) & 0xF0) != 0)
		;

	display_chars(LCD_SEG_L1_3_0, (u8 *) "done", SEG_ON);

}

// *************************************************************************************************
// @fn          InitRadio
// @brief       Inits radio for cSLight
// @param       none
// @return      none
// *************************************************************************************************
void InitRadio(void) {
	// Set the High-Power Mode Request Enable bit so LPM3 can be entered
	// with active radio enabled
	/* PMMCTL0_H = 0xA5;
	 PMMCTL0_L |= PMMHPMRE_L;
	 PMMCTL0_H = 0x00;
	 */
	WriteRfSettings(&cSLrfSettings);
	// Write register settings
//    WriteSingleReg(IOCFG2,   0x0B); // GDO2 output pin config.
//    WriteSingleReg(IOCFG0,   0x06); // GDO0 output pin config.
//    WriteSingleReg(PKTLEN,   0xFF); // Packet length.
	WriteSingleReg(PKTCTRL1, 0x04); // Packet automation control.
	WriteSingleReg(PKTCTRL0, 0x05); // Packet automation control.
	WriteSingleReg(ADDR, 0x01); // Device address.
	WriteSingleReg(CHANNR, 0x00); // Channel number.
//    WriteSingleReg(FSCTRL1,  0x08); // Freq synthesizer control.
//    WriteSingleReg(FSCTRL0,  0x00); // Freq synthesizer control.
	WriteSingleReg(FREQ2, 0x21); // Freq control word, high byte
	WriteSingleReg(FREQ1, 0x71); // Freq control word, mid byte.
	WriteSingleReg(FREQ0, 0x7A); // Freq control word, low byte.
	WriteSingleReg(MDMCFG4, 0x7B); // Modem configuration.
	WriteSingleReg(MDMCFG3, 0x83); // Modem configuration.
	WriteSingleReg(MDMCFG2, 0x13); // Modem configuration.
	WriteSingleReg(MDMCFG1, 0x22); // Modem configuration.
	WriteSingleReg(MDMCFG0, 0xF8); // Modem configuration.
	WriteSingleReg(DEVIATN, 0x42); // Modem dev (when FSK mod en)
//    WriteSingleReg(MCSM1 ,   0x3F); //MainRadio Cntrl State Machine
//    WriteSingleReg(MCSM0 ,   0x18); //MainRadio Cntrl State Machine
//    WriteSingleReg(FOCCFG,   0x1D); // Freq Offset Compens. Config
//    WriteSingleReg(BSCFG,    0x1C); //  Bit synchronization config.
//    WriteSingleReg(AGCCTRL2, 0xC7); // AGC control.
//    WriteSingleReg(AGCCTRL1, 0x00); // AGC control.
//    WriteSingleReg(AGCCTRL0, 0xB2); // AGC control.
//    WriteSingleReg(FREND1,   0xB6); // Front end RX configuration.
//    WriteSingleReg(FREND0,   0x10); // Front end RX configuration.
//    WriteSingleReg(FSCAL3,   0xEA); // Frequency synthesizer cal.
//    WriteSingleReg(FSCAL2,   0x0A); // Frequency synthesizer cal.
//    WriteSingleReg(FSCAL1,   0x00); // Frequency synthesizer cal.
//    WriteSingleReg(FSCAL0,   0x11); // Frequency synthesizer cal.
//    WriteSingleReg(FSTEST,   0x59); // Frequency synthesizer cal.
//    WriteSingleReg(TEST2,    0x88); // Various test settings.
//    WriteSingleReg(TEST1,    0x31); // Various test settings.
//    WriteSingleReg(TEST0,    0x0B); // Various test settings.

	WriteBurstPATable(&cSLpaTable[0], 2);

}

// *************************************************************************************************
// @fn          sx_cSLight
// @brief       cSLight direct function. Button DOWN starts RF commands as long as button is pressed
// @param       u8 line	LINE2
// @return      none
// *************************************************************************************************
//void sx_cSLight(u8 line)
//{
//	//volatile u8 x;
//	//if(button.flag.down||button.flag.up)
//	//{
//		//Display RF symbol
//		display_symbol(LCD_ICON_BEEPER1, SEG_ON);
//		display_symbol(LCD_ICON_BEEPER2, SEG_ON);
//		display_symbol(LCD_ICON_BEEPER3, SEG_ON);
//
//		//Reset radio
//		ResetRadioCore();
//		InitRadio();
//
//	    RF1AIES |= BIT9;
//	    RF1AIFG &= ~BIT9;                         // Clear pending interrupts
//	    RF1AIE &= ~BIT9;                          // Disable TX end-of-packet interrupt
//
//	    cSL_transmit_code();
//	    //Transmit until button is released
//		while(BUTTON_DOWN_IS_PRESSED||BUTTON_UP_IS_PRESSED)
//		{
//			cSL_transmit_code();
//		}
//		Strobe(RF_SIDLE);
//		Strobe(RF_SPWD);
//
//		//Clear RF symbol
//		display_symbol(LCD_ICON_BEEPER1, SEG_OFF);
//		display_symbol(LCD_ICON_BEEPER2, SEG_OFF);
//		display_symbol(LCD_ICON_BEEPER3, SEG_OFF);
//	//}
//}

// *************************************************************************************************
// RF1A extensions to make cSLight run!!!!!!!!!!!!!!!!!!!!!!!!!!
// *************************************************************************************************
// *************************************************************************************************
// @fn          WriteBurstPATable
// @brief       Write to multiple locations in power table
// @param       unsigned char *buffer	Pointer to the table of values to be written
// @param       unsigned char count	Number of values to be written
// @return      none
// *************************************************************************************************
void WriteBurstPATable(unsigned char *buffer, unsigned char count) {
	volatile char i = 0;
	u16 int_state;

	ENTER_CRITICAL_SECTION(int_state);
	while (!(RF1AIFCTL1 & RFINSTRIFG))
		;
	RF1AINSTRW = 0x7E00 + buffer[i];          // PA Table burst write

	for (i = 1; i < count; i++) {
		RF1ADINB = buffer[i];                   // Send data
		while (!(RFDINIFG & RF1AIFCTL1))
			;       // Wait for TX to finish
	}
	i = RF1ADOUTB;            // Reset RFDOUTIFG flag which contains status byte

	while (!(RF1AIFCTL1 & RFINSTRIFG))
		;
	RF1AINSTRB = RF_SNOP;                     // reset PA Table pointer

	EXIT_CRITICAL_SECTION(int_state);
}

// *****************************************************************************
// @fn          WriteRfSettings
// @brief       Write the minimum set of RF configuration register settings
// @param       RF_SETTINGS *pRfSettings  Pointer to the structure that holds the rf settings
// @return      none
// *****************************************************************************
void WriteRfSettings(RF_SETTINGS *pRfSettings) {
	WriteSingleReg(IOCFG2, pRfSettings->iocfg2);
	WriteSingleReg(IOCFG1, pRfSettings->iocfg1);
	WriteSingleReg(IOCFG0, pRfSettings->iocfg0);
	WriteSingleReg(FIFOTHR, pRfSettings->fifothr);
	WriteSingleReg(PKTLEN, pRfSettings->pktlen);
	WriteSingleReg(PKTCTRL1, pRfSettings->pktctrl1);
	WriteSingleReg(PKTCTRL0, pRfSettings->pktctrl0);
	WriteSingleReg(ADDR, pRfSettings->addr);
	WriteSingleReg(CHANNR, pRfSettings->channr);

	WriteSingleReg(FSCTRL1, pRfSettings->fsctrl1);
	WriteSingleReg(FSCTRL0, pRfSettings->fsctrl0);

	WriteSingleReg(FREQ2, pRfSettings->freq2);
	WriteSingleReg(FREQ1, pRfSettings->freq1);
	WriteSingleReg(FREQ0, pRfSettings->freq0);

	WriteSingleReg(MDMCFG4, pRfSettings->mdmcfg4);
	WriteSingleReg(MDMCFG3, pRfSettings->mdmcfg3);
	WriteSingleReg(MDMCFG2, pRfSettings->mdmcfg2);
	WriteSingleReg(MDMCFG1, pRfSettings->mdmcfg1);
	WriteSingleReg(MDMCFG0, pRfSettings->mdmcfg0);
	WriteSingleReg(DEVIATN, pRfSettings->deviatn);

	WriteSingleReg(MCSM2, pRfSettings->mcsm2);
	WriteSingleReg(MCSM1, pRfSettings->mcsm1);
	WriteSingleReg(MCSM0, pRfSettings->mcsm0);
	WriteSingleReg(FOCCFG, pRfSettings->foccfg);
	WriteSingleReg(BSCFG, pRfSettings->bscfg);

	WriteSingleReg(FREND1, pRfSettings->frend1);
	WriteSingleReg(FREND0, pRfSettings->frend0);

	WriteSingleReg(FSCAL3, pRfSettings->fscal3);
	WriteSingleReg(FSCAL2, pRfSettings->fscal2);
	WriteSingleReg(FSCAL1, pRfSettings->fscal1);
	WriteSingleReg(FSCAL0, pRfSettings->fscal0);

	WriteSingleReg(AGCCTRL2, pRfSettings->agcctrl2);
	WriteSingleReg(AGCCTRL1, pRfSettings->agcctrl1);
	WriteSingleReg(AGCCTRL0, pRfSettings->agcctrl0);

}

