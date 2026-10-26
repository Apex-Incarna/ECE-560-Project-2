#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <MKL25Z4.H>

#define UART_OVERSAMPLE_RATE (16)
#define SYS_CLOCK (48e6)
#define BUS_CLOCK (24e6)

// Defines //
#define RING_SIZE 16    // true size is 15; one space is reserved for checking full vs empty
#define COMMAND_SIZE 16 // more than needed but that's okay

// Functions //
void Init_UART2(uint32_t baud_rate);
uint8_t nextIndex(uint8_t index);
void Transmit(char *bufStart, char terminalChar);
void ProcessRingBuf(void);
void ProcessCommand(void);

#endif
// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************
