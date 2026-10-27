#ifndef ADC_H
#define ADC_H

#define SAMPLES_TO_AVERAGE 5

// Functions
void Init_ADC(void);
void Calibrate_ADC(void);
void Convert(void);
float Calculate(void);
void Print(float voltage);

#endif