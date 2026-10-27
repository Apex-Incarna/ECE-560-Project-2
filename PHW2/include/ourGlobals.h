#ifndef GLOBALS_H
#define GLOBALS_H

#include "UART.h"

// UART Globals
extern volatile uint8_t ringBuf[];
extern volatile uint8_t ringHead; // write to the head
extern volatile uint8_t ringTail; // read from the tail

extern char commandBuf[COMMAND_SIZE];

#include "ADC.h"

// ADC Globals
extern volatile int conv_complete;
extern unsigned int res_min, res_max;
extern float P3V3_voltage;
extern volatile int do_convert, do_print, do_calculate;
extern volatile uint32_t ADC_temp;

extern volatile uint16_t dataBuffer[];
extern volatile uint16_t dataBufIndex;
extern volatile uint16_t numSamples;

#endif