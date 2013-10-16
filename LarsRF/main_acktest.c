/* 
 * This file is licensed under BSD. It is originally copyright Texas Instruments,
 * but has been adapted by Lars Kristian Roland
 */

/*
 * Receives an accelerometer signal from the Chronos (modified) and outputs to servos.
 * Using RobG's Servo code.
 */

#include "ti/include.h"

#define SERVO_OUTPUTS BIT0 + BIT1 + BIT2 + BIT3 // P2
#define leftThigh servoPosition[2]
#define rightThigh servoPosition[1]
#define leftFoot servoPosition[0]
#define rightFoot servoPosition[3]

#define	leftThighCenter 100;
#define	rightThighCenter 100;
#define	rightFootCenter 90;
#define	leftFootCenter 90;

unsigned int counter = 0;                   // Servo counter
volatile unsigned int servoPosition[4] = { 90, 90, 90, 90 }; // Default servo position
unsigned int servoOn[4] = { BIT0, BIT1, BIT2, BIT3 };
unsigned char valueIndex = 0;
unsigned int adcValues[4] = { 0, 0, 0, 0 };

void rxPacket();
int convert_acceleration_value(unsigned char value);

extern char paTable[];
extern char paTableLen;

char txBuffer[12];
char rxBuffer[12];
//unsigned int i = 0;

void main(void) {

	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

	// 5ms delay to compensate for time to startup between MSP430 and CC1100/2500
	__delay_cycles(5000);

	RF_init();

	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;

	P2OUT &= ~(SERVO_OUTPUTS);
	P2DIR |= (SERVO_OUTPUTS);             // Port P2.0 - P2.3 servo outs
	P2SEL &= ~(SERVO_OUTPUTS);

	CCTL0 = CCIE;                             // CCR0 interrupt enabled
	CCR0 = 180;                               // ~1.5ms
	TACTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK/8, upmode

	leftThigh= leftThighCenter;
	rightThigh= rightThighCenter;
	rightFoot= rightFootCenter;
	leftFoot= leftFootCenter;

	while (1) {
		_bis_SR_register(LPM0_bits + GIE);        // Enter LPM0 w/ interrupt
		//rxPacket();
	}

}

// Timer A0 interrupt service routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0(void) {
	counter++;                               // Increase counter
	if (counter == 0x05)
		counter = 0; // Counter range is 0-6, the last count is used to add 5ms delay

	P2OUT &= ~(SERVO_OUTPUTS);                       // Clear ports
	if (counter > 0x03) {
		CCR0 = 16
				* (1500
						- (servoPosition[0] + servoPosition[1]
								+ servoPosition[2] + servoPosition[3]));
	} else {
		P2OUT |= servoOn[counter];              // Set port of the current servo
		CCR0 = 16 * 2 * servoPosition[counter]; // Set time for the current servo
	}

}

// The ISR assumes the interrupt came from a pressed button
#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {

	// Clr flag that caused int
	TI_CC_SW_PxIFG = 0x00;
}

// The ISR assumes the interrupt came from GDO0. GDO0 fires indicating that
// CCxxxx received a packet
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void) {
// if GDO fired
	if (TI_CC_GDO0_PxIFG & TI_CC_GDO0_PIN) {
		char status[2];
		char len = 11;

		if (RFReceivePacket(rxBuffer, &len, status)) {

			int x = convert_acceleration_value(rxBuffer[2]);
			int y = convert_acceleration_value(rxBuffer[3]);
			int z = convert_acceleration_value(rxBuffer[4]);
			int w = convert_acceleration_value(rxBuffer[6]);

			leftThigh= 100+(x / 2);
			rightThigh= 100+(x / 2);

			leftFoot = 90+(y / 3);
			rightFoot= 90+(y / 3);

		}
	}

	TI_CC_GDO0_PxIFG &= ~TI_CC_GDO0_PIN;      // After pkt RX, this flag is set.
}

unsigned char acceleration_value_is_positive(unsigned char value) {
	return ((value & BIT7)== 0);
}

int convert_acceleration_value(unsigned char value) {
	int result = (signed char) value;
	return (result);
}
