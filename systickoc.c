#include "systickoc.h"						//we use OC as systick

//hardware configuration
//end hardware configuration

//global defines
//change it for OCx
#define OCxCON1						OC1CON1
#define OCxCON2						OC1CON2
#define OCxTSEL 					OC1CON1bits.OCTSEL	//0=TMR2, 1=TMR3, 2=TMR4, 3=TMR5, 4=TMR1, 7=System Clock
#define OCxOCM 						OC1CON1bits.OCM		//0=disable OC, 3=continuous pulse, 6/7=pwm
#define OCxSYNCSEL					OC1CON2bits.SYNCSEL	//0=not sync'd, 0x0b..03=tmr1..5, 0x1f=this OC
#define OCxR						OC1R				//compare register
#define OCxRS						OC1RS				//period register
#define OCxMD						PMD2bits.OC1MD		//power management register
#define OCxTMR						OC1TMR				//oc1 timer
#define OCxIF						IFS0bits.OC1IF		//interrupt flag
#define OCxIE						IEC0bits.OC1IE		//interrupt enable

//global variables

static void (* /*_tmr2*/_isrptr)(void)=empty_handler;				//tmr1_ptr pointing to empty_handler by default
//static unsigned short _isr_counter=0;	//isr counter
//static unsigned short _tmrocx_prescaler=TMROC_PS_1x;	//default prescaler = 1x
//static unsigned short _ocxpr=0xffff;				//period register
volatile uint32_t systickovf_counter=0;

void _ISR _OC1Interrupt(void) {
	//OCxR += _ocxpr;					//advance the match point
	OCxIF=0;							//clear tmr1 interrupt flag
	systickovf_counter += 0x10000ul;	//increment overcounter
	_isrptr();							//execute user handler
}

//initialize oc1 as timer
//time base: system clock, free running
void systick_init(void) {
	//initialize the handler pointer
	_isrptr = empty_handler;			//reset the handler
	systickovf_counter = 0;				//reset the overflow counter

	//power up the pwm module
	OCxMD = 0;							//0->enable clock, 1->disable clock

	//reset the registers
	OCxCON1 = 0x0000;
	OCxCON2 = 0x0000;

	OCxTMR = 0;							//reset the counter value

	//OCxCON2 register settings
	OCxSYNCSEL = 0x0000;				//0x00: not sync'd to anything, 0x1f: this module as sync source

	//overflow at 0xffff
	OCxRS = 0;							//set period
	OCxR  = 0;							//50% duty cycle

	OCxIF = 0;							//clear the flag
	OCxIE = 1;							//enable the interrupt

	//OCxCON1 register settings
	OCxTSEL = 0x07;						//0=TMR2, 1=TMR3, 2=TMR4, 3=TMR5, 4=TMR1, 7=System Clock
	OCxOCM = 0x03;						//3/5->continue pulse, 6/7 -> edge aligned pwm
}

//activate user isr
void systick_act(void (*isrptr)(void)) {
	/*_tmr2*/_isrptr=isrptr;			//activate the isr handler
	OCxIF = 0;							//reset the flag
	OCxIE = 1;							//rtc1 interrupt on
}

//return timer clicks
//execution time = 30 ticks, no optimization
uint32_t systicks(void) {
	uint32_t m;
	uint16_t f;

	//do a double read
	do {
		m = systickovf_counter;			//read the overflow counter
		f = OCxTMR;						//read the least significant 16-bits
	} while (m != systickovf_counter);	//gaurd against overflow

	return (m | f) << 0;
}
