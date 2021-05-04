#ifndef SYSTICKOC_H_INCLUDED
#define SYSTICKOC_H_INCLUDED

//use dedicated timer on OC for systick / timer

#include "gpio.h"							//we use gpio

//hardware configuration
//end hardware configuration

//global defines

//global variables

//initialize oc1 as timer
//time base: system clock, free running
void systick_init(void);

//activate user isr
void systick_act(void (*isrptr)(void));

//read 32-bit systick
uint32_t systicks(void);

#endif // SYSTICKOC_H_INCLUDED
