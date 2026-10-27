/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "UART.h"
#include "DAC.h"
#include "timers.h"
#include "delay.h"
#include "ourGlobals.h"

#define SAMPLES_TO_AVERAGE (25)

void Misc(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[12] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[12] |= PORT_PCR_MUX(1);
	GPIOA->PDDR |= 1 << 12;
	GPIOA->PCOR = 1 << 12;
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main(void)
{
	Init_RGB_LEDs();
	Control_RGB_LEDs(1, 1, 0); // Yellow
	Misc();
	Calibrate_ADC();
	Init_ADC();
	Init_UART2(57600);
	Control_RGB_LEDs(1, 0, 0); // Red
	Init_DAC();
	Control_RGB_LEDs(0, 1, 1); // Cyan
	// Init_LPTMR(1000);
	Init_PIT();
	Control_RGB_LEDs(1, 0, 1); // Magenta
	printf("\n\rPlease debug me!\n\r");
	Control_RGB_LEDs(0, 1, 0); // Green
	Delay(3000);

	while (1)
	{
		ProcessRingBuf();

		if (do_convert > 0)
		{
			do_convert--;
			Convert();
		}
		if (do_calculate > 0)
		{
			do_calculate--;
			P3V3_voltage = Calculate();
		}
		if (do_print > 0)
		{
			Print(P3V3_voltage);
			do_print = 0;
		}
	}
}
