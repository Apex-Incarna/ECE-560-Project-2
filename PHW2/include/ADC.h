#ifndef ADC_H
#define ADC_H

#define SAMPLES_TO_AVERAGE 5
#define DATA_BUF_MAX 2000

#define ADC_THRESHOLD 9929  // (9929+1)/65536 = 0.50002

// Functions
void Init_ADC(void);
void Calibrate_ADC(void);
float Calculate(void);
void Print(float voltage);
void Set_ADC_Periodic_Sampling(void);
void Set_ADC_Waiting_Trigger(void);

#endif