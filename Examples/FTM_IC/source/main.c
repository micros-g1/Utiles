/*
 * main.c
 *
 *  Created on: May 1, 2015
 *      Author: Juan Pablo VEGA - Laboratorio de Microprocesadores
 */

#include "hardware.h"
#include "PORT.h"
#include "GPIO.h"
#include "FTM.h"


#define __FOREVER__ 	for(;;)

#define PIN_RED_LED 		22     	//PTB22
#define PIN_BLUE_LED 		21     	//PTB21
#define PIN_GREEN_LED 		26 	   	//PTE26
#define PIN_PUSH_BUTTON  	4 		//PTA4


#define BALIZA_DELAY	4000000UL

void idle(void);
void delayLoop (uint32_t veces);

int main (void)
{


 	 	 	 	hw_Init ();
 	 	 	 	PORT_Init();
 	 	 		GPIO_Init();
 	 	 		FTM_Init();


 	 	// 		hw_DisableInterrupts();

 	 	 		__FOREVER__;

			// Enable interrupts
			//hw_EnableInterrupts();






}


__ISR__  PORTA_IRQHandler(void)
{

	// Clear port IRQ flag


		PORTA->PCR[4] |= PORT_PCR_ISF_MASK;

		PTB->PCOR = (1<<21)|(1<<22);



}


void idle(void)
{

}


void delayLoop (uint32_t veces)
{
	while (veces--);
}


__ISR__  SysTick_Handler (void)
{
	static uint32_t speed=4;  // 0.5 seg @ tick =125ms

	if (speed==0)
	{

		PTB->PTOR = (1<<21)|(1<<22);
		speed=4;

	}

	speed--;


}

void SysTick_Init (void)
{
	SysTick->CTRL = 0x00;
	SysTick->LOAD = 12500000L-1; //12499999L; // <= 125 ms @ 100Mhz
	SysTick->VAL  = 0x00;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}


