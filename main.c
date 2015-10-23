/*Justin Johnson Z23136514
 * FAU Embedded Systems Design, Fall 2015
 *
 * Bluetooth Echo
 * Baudrate = 9600
 * RXD = P1.1
 * TXD = P1.2
 */
#include <msp430g2253.h>


void sendMessage(unsigned char Tx_data);	//Assigns 1 byte to UCA0TXBUF and transmits data

unsigned char Rx_Data = 0;					//Variable to store incoming RX
unsigned char Tx_Data = 0;					//Variable to store outgoing TX

int main(void){
  WDTCTL = WDTPW + WDTHOLD;				//Stop WDT
  /*** Set-up system clocks ***/
  if (CALBC1_1MHZ == 0xFF){				// If calibration constant erased
    while (1);							// do not load, trap CPU!
  }
  DCOCTL = 0;							// Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;				// Set DCO
  DCOCTL = CALDCO_1MHZ;

  /*** Set-up GPIO ***/
  P1SEL = BIT1 + BIT2;					// P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2;
  P1DIR |= BIT6 + BIT0;					// P1.1 and P1.6 set as output
  P1OUT &= ~(BIT6 + BIT0);				// P1.1 and P1.6 OFF

  /*** Set-up USCI A ***/
  UCA0CTL1 |= UCSSEL_2;					// SMCLK
  UCA0BR0 = 104;						// 1MHz 9600
  UCA0BR1 = 0;							// 1MHz 9600
  UCA0MCTL = UCBRS0;					// Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;					// Initialize USCI state machine
  IE2 |= UCA0RXIE;						// Enable USCI_A0 RX interrupt

  __bis_SR_register(LPM0_bits + GIE);	// Enter LPM0 with interrupts enabled

}


//Send Tx_Data through TX BUFFER
void sendMessage(unsigned char Tx_Data){
  while (!(IFG2&UCA0TXIFG));			// USCI_A0 TX buffer ready?
  UCA0TXBUF = Tx_Data;					//Send Tx_data
  Tx_Data = 0;							//clear Tx_Data
}

//USCI A interrupt handler fires when data is received
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
  Rx_Data = UCA0RXBUF;				// Assign received byte to Rx_Data from buffer
  Tx_Data = Rx_Data;
  sendMessage(Tx_Data);				//echo data
}
