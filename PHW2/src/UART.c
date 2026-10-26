#include "UART.h"
#include <stdio.h>
#include "ourGlobals.h"

#if 0
struct __FILE
{
  int handle;
};
#endif

FILE __stdout; // Use with printf
FILE __stdin;  // use with fget/sscanf, or scanf

volatile char ringBuf[RING_SIZE];
volatile uint8_t ringHead = 0; // write to the head (owned by ISR)
volatile uint8_t ringTail = 0;	// read from the tail (owned by main loop)

char commandBuf[COMMAND_SIZE];
uint8_t commandIndex = 0;

// Retarget the fputc method to use the UART2
int fputc(int ch, FILE *f)
{
	while (!(UART2->S1 & UART_S1_TDRE_MASK) && !(UART2->S1 & UART_S1_TC_MASK))
		;
	UART2->D = ch;
	return ch;
}

// Retarget the fgetc method to use the UART2
int fgetc(FILE *f)
{
	while (!(UART2->S1 & UART_S1_RDRF_MASK))
		;
	return UART2->D;
}

void Init_UART2(uint32_t baud_rate)
{
	uint16_t sbr;
	uint8_t temp;

	// Enable clock gating for UART2 and Port D
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

	// Make sure transmitter and receiver are disabled before init
	UART2->C2 &= ~UART_C2_TE_MASK & ~UART_C2_RE_MASK;

	// Set pins D2, D3 to UART2 Rx and Tx
	PORTD->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(3); // Rx
	PORTD->PCR[3] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(3); // Tx

	// Set baud rate, oversampling ratio, stop bits
	sbr = (uint16_t)(((BUS_CLOCK) / (baud_rate * UART_OVERSAMPLE_RATE)) + 0.5);
	UART2->BDH = UART_BDH_SBNS(1);

	// Buggy code
	// UART2->BDH |= UART_BDH_SBR(0);
	// UART2->BDL = UART_BDL_SBR(5);

	UART2->BDH |= UART_BDH_SBR(sbr >> 8);
	UART2->BDL = UART_BDL_SBR(sbr);

	// Disable interrupts for RX active edge and LIN break detect, select one stop bit
	UART2->BDH |= UART_BDH_RXEDGIE(0) | UART_BDH_SBNS(0) | UART_BDH_LBKDIE(0);

	// Don't enable loopback mode, use 8 data bit mode, don't use parity
	UART2->C1 = UART_C1_LOOPS(0) | UART_C1_M(0) | UART_C1_PE(0);
	// Don't invert transmit data, don't enable interrupts for errors
	UART2->C3 = UART_C3_TXINV(0) | UART_C3_ORIE(0) | UART_C3_NEIE(0) | UART_C3_FEIE(0) | UART_C3_PEIE(0);

	/*
	// Clear error flags
	UART2->S1 |= (uint8_t) (UART_S1_OR_MASK | UART_S1_NF_MASK |
									UART_S1_FE_MASK | UART_S1_PF_MASK);
	*/

	// Do not invert received data
	UART2->S2 = UART_S2_RXINV(0);

	// The textbook says we need this code for the UART IRQ to work
	NVIC_SetPriority(UART2_IRQn, 2); // 0, 1, 2, or 3
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);

	// Enable UART receive interrupt
	UART2->C2 |= UART2_C2_RIE(1);

	// Enable UART receiver and transmitter
	UART2->C2 |= UART_C2_RE(1) | UART_C2_TE(1);

	// Clear the UART RDRF flag
	temp = UART2->S2;
	temp = UART2->D;
}

void UART2_IRQHandler(void)
{
	if (UART2->S1 & UART_S1_RDRF_MASK) {	// Received Char Interrupt
		char cin = UART2->D; // reading this clears the interrupt flag

		uint8_t nextHead = nextIndex(ringHead);
		if (nextHead == ringTail) {
			// Ring Full! Do not proceed to write the next character
			// TODO: Turn on RED Led
			return;
		}

		ringBuf[ringHead] = cin; // Reads the receieved character into ringBuf
		ringHead = nextIndex(ringHead);
	}
}

// // Sends every character in the buffer starting with the address bufStart up to and including the location of the specified terminalChar character
// void Transmit(char *bufStart, char terminalChar)
// {
// 	int index = 0;

// 	char currentChar;

// 	while (currentChar = bufStart[index++] != terminalChar)
// 	{
// 		fputc(currentChar);
// 	}

// 	fputc(bufStart[terminalChar]); // Send the terminal character
// }