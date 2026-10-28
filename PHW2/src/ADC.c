#include <MKL25Z4.H>
#include "ourGlobals.h"
#include "timers.h"

unsigned int res_min = 0xffff, res_max = 0;
volatile int conv_complete = 0;
float P3V3_voltage = 0.0;
volatile int do_convert = 0, do_print = 0, do_calculate = 0;
volatile uint32_t ADC_temp = 0;

// Data Buffer
volatile uint16_t dataBuffer[DATA_BUF_MAX];
volatile uint16_t dataBufIndex = 0;
volatile uint16_t numSamples = 1;
volatile uint8_t ADC_Channel = 12;
volatile uint8_t triggered = 0;

void Init_ADC(void)
{
	SIM->SCGC6 |= SIM_SCGC6_ADC0(1); // Activates the clock for the ADC

	ADC0->CFG1 = ADC_CFG1_ADLPC(0) 		// Do not use low power
				| ADC_CFG1_ADIV(1) 		// Divide clock by 2, results in 12 Mhz ADC Clock
				| ADC_CFG1_ADICLK(0) 	// Select Bus clock as clock source
				// | ADC_CFG1_ADLSMP(1) 	// Use long sample time to increase acccuracy
				| ADC_CFG1_MODE(3);		// Use 16-bit conversion

	ADC0->CFG2 = ADC_CFG2_ADLSTS(3)		// Use the shortest Long Sample Time to increase conversion speed
				| ADC_CFG2_ADHSC(1) 	// Use High-speed configuration since we are running it so fast (required by datasheet)
				| ADC_CFG2_ADACKEN(0) 	// Do not use asynchronous clock output enable
				| ADC_CFG2_MUXSEL(0);	// Select the ADC A channels (rather than B)

	ADC0->SC2 = ADC_SC2_ADTRG(1) 		// Use hardware trigger
				| ADC_SC2_REFSEL(0);	// Default voltage reference

	ADC0->SC3 = ADC_SC3_AVGE(0); // Do not use hardware averaging

	// Configure PIT to trigger the ADC
	SIM->SOPT7 = SIM_SOPT7_ADC0ALTTRGEN(1)	// allow alternate trigger for ADC
				| SIM_SOPT7_ADC0TRGSEL(4);  // 4 = PIT Trigger 0

	// Since Hardware trigger is set, this does not start a conversion
    ADC0->SC1[0] = ADC_SC1_ADCH(12) 	// set default input channel to 12

	ADC0->CV1 = ADC_THRESHOLD;	// Set the threshold for ADC hardware comparison (for waiting trigger mode)

	// Configure NVIC
	NVIC_SetPriority(ADC0_IRQn, 0); // Priorities 0-3, 0 is highest
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	NVIC_EnableIRQ(ADC0_IRQn);

	// Enable bandgap buffer (We do not need this)
	// PMC->REGSC |= PMC_REGSC_BGBE_MASK;
}

void Set_ADC_Waiting_Trigger(void) {
    ADC0->SC1[0] = ADC_SC1_ADCH(31);	// Disables the channel to abort any ongoing conversion

    ADC0->SC2 = ADC_SC2_ACFE_MASK		// Compare Function Enable
				| ADC_SC2_ACFGT_MASK	// Compare Function Greater Than Enable
				| ADC_SC2_ADTRG(0); 	// Software trigger: start conversions by writing to SC1A

    ADC0->SC3 = ADC_SC3_ADCO_MASK;	// Continuous Conversion Enable

	triggered = 0;	// reset trigger flag
	
    ADC0->SC1[0] = ADC_SC1_AIEN_MASK	// Enable conversion-complete interrupt and start conversions
				   | ADC_SC1_ADCH(ADC_Channel);	// Set channel back to current channel
}

void Set_ADC_Periodic_Sampling(void) {
	ADC0->SC1[0] = ADC_SC1_ADCH(31);	// Disables the channel to abort any ongoing conversion

	ADC0->SC3 = 0;	// Disable Continuous Conversion

    ADC0->SC2 = ADC_SC2_ADTRG(1);	// Disable Compare Function, enable Hardware trigger

	ADC0->SC1[0] = ADC_SC1_AIEN_MASK	// Enable conversion-complete interrupt (this will not start a conversion since Hardware trigger mode)
				   | ADC_SC1_ADCH(ADC_Channel);	// Set channel back to current channel
}

void ADC0_IRQHandler(void)
{
	uint16_t ADCResult = ADC0->R[0];	// automatically clears interrupt flag
	
	if (!triggered) {
		// This is the first data point after the trigger

		Start_PIT();
		Set_ADC_Periodic_Sampling();
		triggered = 1;
	}

	dataBuffer[dataBufIndex++] = ADCResult;
	if (dataBufIndex >= numSamples) {
		Stop_PIT(); // Stop converting
		dataBufIndex = 0; // Reset buffer index
		
		// TODO: set a flag to say finished data acquisition?
	}
}

void Calibrate_ADC(void)
{
	SIM->SCGC6 |= SIM_SCGC6_ADC0(1);
	// From SRM section 28.4.6

	// Set fADCK <= 4 MHz. Not low power, divide clock by 8, select bus clock, long sample time, 16-bit conversion
	// With CLK_SETUP == 1, Bus clock = 24 MHz, so fADCK = 24 MHz/8 = 3 MHz
	ADC0->CFG1 = ADC_CFG1_ADLPC(0) | ADC_CFG1_ADIV(3) | ADC_CFG1_ADICLK(0) |
				 ADC_CFG1_ADLSMP(1) | ADC_CFG1_MODE(3);
	// longest sample time, normal conversion speed, async clock output disabled, ADxxa channels selected
	ADC0->CFG2 = ADC_CFG2_ADLSTS(0) | ADC_CFG2_ADHSC(0) | ADC_CFG2_ADACKEN(0) |
				 ADC_CFG2_MUXSEL(0);
	// Set maximum hardware averaging: AVGE = 1, AVGS = 11
	ADC0_SC3 = ADC_SC3_AVGS(1) | ADC_SC3_AVGS(3);
	// Use Vdda reference, software ADC trigger
	ADC0->SC2 = ADC_SC2_REFSEL(1) | ADC_SC2_ADTRG(0);

	// Start calibration
	ADC0_SC3 |= ADC_SC3_CAL(1);
	// Await calibration completion
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
		;

	if (ADC0_SC3 & ADC_SC3_CALF_MASK)
	{
		// calibration failed
		Control_RGB_LEDs(1, 0, 0);
		while (1)
			;
	}

	// Read calibration results, assign gains
	uint16_t pcal;
	pcal = ADC0_CLP0 + ADC0_CLP1 + ADC0_CLP2 + ADC0_CLP3 + ADC0_CLP4 + ADC0_CLPS;
	pcal /= 2;
	pcal |= 0x8000;
	ADC0_PG = pcal;

	uint16_t mcal;
	mcal = ADC0_CLM0 + ADC0_CLM1 + ADC0_CLM2 + ADC0_CLM3 + ADC0_CLM4 + ADC0_CLMS;
	mcal /= 2;
	mcal |= 0x8000;
	ADC0_MG = mcal;
}


float Calculate(void)
{
	float vrail;
	uint32_t result;

	result = ADC_temp; // ADC0->R[0];
	// Update minimum and maximum results
	if (result < res_min)
		res_min = result;
	if (result > res_max)
		res_max = result;
	vrail = (1.0 / result) * 4096;
	do_print++;
	return vrail;
}

void Print(float voltage)
{
	static float sum = 0.0;
	static int count = 0;

	// Make LED red while printing out through serial port
	Control_RGB_LEDs(1, 2, 2);
	// Print out [minimum|maximum] range
	// printf("[%5d <-%4d -> %5d] %4d. ", res_min, res_max - res_min, res_max);
	printf("P3V3 Rail is at %6.4f V", voltage);

	sum += voltage;
	if (++count == SAMPLES_TO_AVERAGE)
	{
		printf(" --- Average %f over %d samples\n\r", sum / count, SAMPLES_TO_AVERAGE);
		sum = 0.0;
		count = 0;
	}
	else
		printf("\n\r");

	Control_RGB_LEDs(0, 2, 2);
}