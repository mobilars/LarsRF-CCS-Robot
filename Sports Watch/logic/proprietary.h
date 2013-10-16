/*
 * proprietary.h
 *
 *  Created on: 14. okt. 2013
 *      Author: Lars
 */

#ifndef PROPRIETARY_H_
#define PROPRIETARY_H_


void transmit_proprietary(u8 x, u8 y, u8 z);
void init_proprietary();

typedef struct {
    BYTE iocfg2;        // GDO2 Output Configuration
    BYTE iocfg1;        // GDO1 Output Configuration
    BYTE iocfg0;        // GDO0 Output Configuration
    BYTE fifothr;       // RX FIFO and TX FIFO Thresholds
    BYTE sync1;         // Sync Word, High Byte
    BYTE sync0;         // Sync Word, Low Byte
    BYTE pktlen;        // Packet Length
    BYTE pktctrl1;      // Packet Automation Control
    BYTE pktctrl0;      // Packet Automation Control
    BYTE addr;          // Device Address
    BYTE channr;        // Channel Number
    BYTE fsctrl1;       // Frequency Synthesizer Control
    BYTE fsctrl0;       // Frequency Synthesizer Control
    BYTE freq2;         // Frequency Control Word, High Byte
    BYTE freq1;         // Frequency Control Word, Middle Byte
    BYTE freq0;         // Frequency Control Word, Low Byte
    BYTE mdmcfg4;       // Modem Configuration
    BYTE mdmcfg3;       // Modem Configuration
    BYTE mdmcfg2;       // Modem Configuration
    BYTE mdmcfg1;       // Modem Configuration
    BYTE mdmcfg0;       // Modem Configuration
    BYTE deviatn;       // Modem Deviation Setting
    BYTE mcsm2;         // Main Radio Control State Machine Configuration
    BYTE mcsm1;         // Main Radio Control State Machine Configuration
    BYTE mcsm0;         // Main Radio Control State Machine Configuration
    BYTE foccfg;        // Frequency Offset Compensation Configuration
    BYTE bscfg;         // Bit Synchronization Configuration
    BYTE agcctrl2;      // AGC Control
    BYTE agcctrl1;      // AGC Control
    BYTE agcctrl0;      // AGC Control
    BYTE worevt1;       // High Byte Event0 Timeout
    BYTE worevt0;       // Low Byte Event0 Timeout
    BYTE worctrl;       // Wake On Radio Control
    BYTE frend1;        // Front End RX Configuration
    BYTE frend0;        // Front End TX Configuration
    BYTE fscal3;        // Frequency Synthesizer Calibration
    BYTE fscal2;        // Frequency Synthesizer Calibration
    BYTE fscal1;        // Frequency Synthesizer Calibration
    BYTE fscal0;        // Frequency Synthesizer Calibration
    BYTE fstest;        // Frequency Synthesizer Calibration Control
    BYTE ptest;         // Production Test
    BYTE agctest;       // AGC Test

} RF_SETTINGS;


#endif /* PROPRIETARY_H_ */
