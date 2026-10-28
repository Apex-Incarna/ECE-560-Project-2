/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include "ourGlobals.h"

#define SAMPLES_TO_AVERAGE (25)

volatile uint8_t state = CLI_STATE;

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
	// Init_DAC(); Not needed for us
	Control_RGB_LEDs(0, 1, 1); // Cyan
	// Init_LPTMR(1000);
	Init_PIT();
	Control_RGB_LEDs(1, 0, 1); // Magenta
	printf("\n\rPlease debug me!\n\r");
	Control_RGB_LEDs(0, 1, 0); // Green
	Delay(3000);

	printf("\n> "); // Initial >

	while (1)
	{
		switch (state)
		{
		case CLI_STATE:
			ProcessRingBuf();
			break;

		case AWAIT_STATE:
			/* code */
			break;

		case RECORD_STATE:
			/* code */
			break;

		case SEND_STATE:
			/* code */
			break;

		default:
			break;
		}
	}
}
