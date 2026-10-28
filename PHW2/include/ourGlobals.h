#ifndef GLOBALS_H
#define GLOBALS_H

#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "UART.h"
#include "DAC.h"
#include "ADC.h"
#include "timers.h"
#include "delay.h"

// UART Globals
extern volatile char ringBuf[];
extern volatile uint8_t ringHead; // write to the head
extern volatile uint8_t ringTail; // read from the tail

extern char commandBuf[COMMAND_SIZE];

// ADC Globals
extern volatile int conv_complete;
extern unsigned int res_min, res_max;
extern float P3V3_voltage;
extern volatile int do_convert, do_print, do_calculate;
extern volatile uint32_t ADC_temp;

extern volatile uint16_t dataBuffer[];
extern volatile uint16_t dataBufIndex;
extern volatile uint16_t numSamples;
extern volatile uint8_t ADC_Channel;

// State Globals
extern volatile uint8_t state;
#define CLI_STATE (0x00)
#define AWAIT_STATE (0x01)
#define RECORD_STATE (0x02)
#define SEND_STATE (0x03)

#endif