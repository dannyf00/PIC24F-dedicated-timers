#include "config.h"						//configuration words - for C30. Use config.h for XC16
#include "gpio.h"
#include "delay.h"						//we use software delays
#include "systickoc.h"					//include systick on OC modules
#include "uart1.h"						//we use uart1
#include "uart2.h"						//we use uart2
#include <string.h>						//we use strcpy

//hardware configuration
#define LED_PORT			PORTB
#define LED_DDR				TRISB
#define LED					(1<<5)
#define LED_CNT				(1234567ul * 2 + 0*F_CPU / 1)
//end hardware configuration

//global defines

//global variables

//print uRAM buffer
void uprint(char *str, uint32_t dat) {
	char uRAM[50];						//uram print buffer

	strcpy(uRAM, str);					//copy str to uart

	uRAM[19] = '0' + (dat % 10); dat /= 10;
	uRAM[18] = '0' + (dat % 10); dat /= 10;
	uRAM[17] = '0' + (dat % 10); dat /= 10;
	uRAM[16] = ',';						//0' + (dat % 10); dat /= 10;
	uRAM[15] = '0' + (dat % 10); dat /= 10;
	uRAM[14] = '0' + (dat % 10); dat /= 10;
	uRAM[13] = '0' + (dat % 10); dat /= 10;
	uRAM[12] = ',';						//0' + (dat % 10); dat /= 10;
	uRAM[11] = '0' + (dat % 10); dat /= 10;
	uRAM[10] = '0' + (dat % 10); dat /= 10;
	uRAM[ 9] = '0' + (dat % 10); dat /= 10;
	uRAM[ 8] = ',';						//0' + (dat % 10); dat /= 10;
	uRAM[ 7] = '0' + (dat % 10); dat /= 10;
	uart2_puts(uRAM);					//send str to uart
}

int main(void) {
	uint32_t cnt=0, tmp, tmp1;
	uint32_t octmr0, octmr1;

	mcu_init();							//reset the mcu
	IO_OUT(LED_DDR, LED);				//led as output

	//initialize systick
	systick_init();

	//initialize oc2
	PMD2bits.OC2MD = 0;					//enable power
	OC2CON1bits.OCTSEL = 0x07;			//use system clock
	OC2CON1bits.OCM    = 0x03;			//continuous pulse
	OC2CON2bits.SYNCSEL= 0;				//sync to nothing

	//initialize uart2
	uart2_init(UART_BR_9600);			//initialize uart2

	cnt = LED_CNT;						//reset the systick
	ei();
	while (1) {
		//delay_ms(200);
		//if enough time has passed
		if ((tmp=systicks()) > cnt) {
			IO_FLP(LED_PORT, LED);		//flip the led pin

			//update ocxtmr
			octmr1=OC1TMR;				//read current timer value
			tmp1=octmr1-octmr0;
			octmr0=octmr1;
			//print data
			uprint("cnt  =                   ", cnt);			//print cnt
			uprint("tick =                   ", tmp);			//print cnt
			//uprint("oc1t =                   ", OC1TMR);		//print oc1trm
			uprint("tmp1 =                   ", tmp1 & 0xffff);		//print oc1trm
			uprint("oc2t =                   \r\n", OC2TMR);	//print oc2tmr

			cnt += LED_CNT;				//advance to the next match point
		}
	}
}
