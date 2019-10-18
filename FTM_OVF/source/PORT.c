
#include "PORT.h"


void PORT_Init (void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;


	PORT_ClearPortFlags (PORTA);
	PORT_ClearPortFlags (PORTB);
	PORT_ClearPortFlags (PORTC);
	PORT_ClearPortFlags (PORTD);
	PORT_ClearPortFlags (PORTE);

	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTB_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);
	NVIC_EnableIRQ(PORTE_IRQn);

	/*
	 * TO DO
	 */
}








void PORT_SetPullResistorMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_PE_MASK;
	port->PCR[bit] |= PORT_PCR_PE(mode);
}

void PORT_SetPullUpMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_PS_MASK;
	port->PCR[bit] |= PORT_PCR_PS(mode);
}

void PORT_SetOpenDrainMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_ODE_MASK;
	port->PCR[bit] |= PORT_PCR_ODE(mode);
}

void PORT_SetPassiveFilterMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_PFE_MASK;
	port->PCR[bit] |= PORT_PCR_PFE(mode);
}

void PORT_SetSlowSlewRateMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_SRE_MASK;
	port->PCR[bit] |= PORT_PCR_SRE(mode);
}

void PORT_SetHighDriveStrengthMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_DSE_MASK;
	port->PCR[bit] |= PORT_PCR_DSE(mode);
}

void PORT_SetMuxMode (PORT_t port, PORTBit_t bit, PORTMux_t mux)
{
	port->PCR[bit] &= ~PORT_PCR_MUX_MASK;
	port->PCR[bit] |= PORT_PCR_MUX(mux);
}

void PORT_SetInterruptMode (PORT_t port, PORTBit_t bit, PORTEvent_t event)
{
	port->PCR[bit] &= ~PORT_PCR_IRQC_MASK;
	port->PCR[bit] |= PORT_PCR_IRQC(event);
}

bool PORT_IsInterruptPending (PORT_t port, PORTBit_t bit)
{
	return port->PCR[bit] & PORT_PCR_ISF_MASK;
}

void PORT_ClearInterruptFlag (PORT_t port, PORTBit_t bit)
{
	port->PCR[bit] |= PORT_PCR_ISF_MASK;
}

void PORT_Configure (PORT_t port, PORTBit_t bit, bool pr, bool pu, bool od, bool pf, bool ssr, bool hds, PORTMux_t mux, PORTEvent_t event)
{



	port->PCR[bit] = PORT_PCR_PE(pr) |
			         PORT_PCR_PS(pu) |
					 PORT_PCR_ODE(od) |
					 PORT_PCR_PFE(pf) |
					 PORT_PCR_SRE(ssr) |
					 PORT_PCR_DSE(hds) |
					 PORT_PCR_MUX(mux) |
					 PORT_PCR_IRQC(event) |
					 PORT_PCR_ISF_MASK;


}


// This is a to do function see led.c
void PORT_Configure2 (PORT_t port, PORTBit_t bit,PCRstr UserPCR)
{

	port->PCR[bit]=UserPCR.PCR ;

}






PORTFlags_t PORT_GetPortFlags (PORT_t port)
{
	return port->ISFR & PORT_ISFR_ISF_MASK;
}

void PORT_ClearPortFlags (PORT_t port)
{
	port->ISFR = PORT_ISFR_ISF_MASK;
}

