#ifndef TIMERS_H
#define TIMERS_H
#include "MKL25Z4.h"

#define FREQ_LPO (1000)
#define PIT_CLOCK_MHZ (24) // 24 MHz
// TODO: Verify this is the correct frequency

#define WAIT_PERIOD 80

extern volatile int not_ready;
extern volatile int enable;

void Init_LPTMR(uint32_t freq);
void Start_LPTMR(void);
void Stop_LPTMR(void);

void Init_PIT(void);
void Set_PIT_Period(uint16_t period);
void Start_PIT(void);
void Stop_PIT(void);

void Init_PWM(void);
void Set_PWM_Values(uint16_t perc1, uint16_t perc2);


#endif
// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
