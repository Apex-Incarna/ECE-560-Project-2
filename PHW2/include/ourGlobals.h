#ifndef GLOBALS_H
#define GLOBALS_H

// UART Globals
extern volatile uint8_t ringBuf[];
extern volatile uint8_t ringHead; // write to the head
extern volatile uint8_t ringTail; // read from the tail

extern char commandBuf[COMMAND_SIZE];

#endif