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
void Send_Data(void);

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

// LED Globals
#define SET_LED_OFF Control_RGB_LEDs(0, 0, 0)
#define SET_LED_RED Control_RGB_LEDs(1, 0, 0)
#define SET_LED_GREEN Control_RGB_LEDs(0, 1, 0)
#define SET_LED_BLUE Control_RGB_LEDs(0, 0, 1)
#define SET_LED_YEL Control_RGB_LEDs(1, 1, 0)
#define SET_LED_MAG Control_RGB_LEDs(1, 0, 1)
#define SET_LED_CYAN Control_RGB_LEDs(0, 1, 1)
#define SET_LED_WHITE Control_RGB_LEDs(1, 1, 1)

// Timer Globals
#define DEFAULT_PERIOD 100

#endif