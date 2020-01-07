
#include "FTM.h"
#include "PORT.h"
#include "GPIO.h"
#include "FTM.h"



void IC_Init(void);
void OC_ISR(void);
void IC_ISR(void);


uint16_t OC_Delta = 200;


/* FTM0 fault, overflow and channels interrupt handler*/

__ISR__ FTM0_IRQHandler(void)
{
	OC_ISR();
}

/* FTM3 fault, overflow and channels interrupt handler*/

__ISR__ FTM3_IRQHandler(void)
{
	IC_ISR();
}

void OC_ISR(void)  //FTM0 CH0 PTC8 as GPIO
{
	FTM_ClearInterruptFlag (FTM0, FTM_CH_0);
	GPIO_Toggle(PTC, 1 << 8);
	FTM_SetCounter(FTM0,FTM_CH_0,FTM_GetCounter (FTM0, FTM_CH_0)+OC_Delta); // OC + oc_Delta --> oc
}

void IC_ISR(void) //FTM3 CH5 PTC9 as IC
{
	static uint16_t med1,med2,med;
	static uint8_t  state=0;

	FTM_ClearInterruptFlag (FTM3, FTM_CH_5);


	if(state==0)
	{
		med1=FTM_GetCounter (FTM3, FTM_CH_5); //
		state=1;
	}
	else if(state==1)
	{
		med2=FTM_GetCounter (FTM3, FTM_CH_5);
		med=med2-med1;
		state=0;					// Set break point here and watch "med" value
	}


}


void FTM_Init (void)
{
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;

	NVIC_EnableIRQ(FTM0_IRQn);
	NVIC_EnableIRQ(FTM1_IRQn);
	NVIC_EnableIRQ(FTM2_IRQn);
	NVIC_EnableIRQ(FTM3_IRQn);

	FTM0->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM1->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM2->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM3->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;

	/*
	 * TO DO
	 */

	IC_Init();
}

/// FTM Input Capture Example

// To Test Connect PC9(IC)-PC8(GPIO)
// or PC9(IC)-PC1(OC)


void IC_Init (void)
{
	// PTC1 as OC (FTM0-CH0)
	PCRstr UserPCR;

	UserPCR.PCR=false;			// Default All false, Set only those needed

	UserPCR.FIELD.DSE=true;
	UserPCR.FIELD.MUX=PORT_mAlt4;
	UserPCR.FIELD.IRQC=PORT_eDisabled;

	PORT_Configure2 (PORTC,1,UserPCR);

	// PTC8 as GPIO
	UserPCR.PCR=false;			// Default All false, Set only those needed

	UserPCR.FIELD.DSE=true;
	UserPCR.FIELD.MUX=PORT_mGPIO;
	UserPCR.FIELD.IRQC=PORT_eDisabled;

	PORT_Configure2 (PORTC,8,UserPCR);

	GPIO_SetDirection(PTC, 8, GPIO__OUT);


	// PTC9 as IC (FTM3-CH5)
	UserPCR.PCR=false;			// Default All false, Set only those needed

	UserPCR.FIELD.DSE=true;
	UserPCR.FIELD.MUX=PORT_mAlt3;
	UserPCR.FIELD.IRQC=PORT_eDisabled;

	PORT_Configure2 (PORTC,9,UserPCR);

	//  Enable Timer advanced modes (FTMEN=1)

	FTM0->MODE=(FTM0->MODE & ~FTM_MODE_FTMEN_MASK) | FTM_MODE_FTMEN(1);

	/// BusClock=sysclk/2= 50MHz
	/// Set prescaler = divx32 => Timer Clock = 32 x (1/BusClock)= 32x1/50MHz= 0.64 useg
	/// =>Timer Clock x OC_Delta= 0.64 x 100=64 useg

	/// ---- Generador

	FTM_SetPrescaler(FTM0, FTM_PSC_x32);                     // Set Prescaler = divx32
	FTM0->CNTIN=0x0000;				  		                 // Free running
	FTM0->MOD=0xFFFF;
	FTM_SetWorkingMode(FTM0,FTM_CH_0,FTM_mOutputCompare);    // OC Function
	FTM_SetOutputCompareEffect (FTM0, FTM_CH_0,FTM_eToggle); // Toggle Output Pin
	FTM_SetInterruptMode (FTM0,FTM_CH_0, true); 		     // enable interrupts
	FTM_StartClock(FTM0);                                    // Select BusClk

	//--- medidor

	FTM_SetPrescaler(FTM3, FTM_PSC_x32);	 				// Set Prescaler = divx32
	FTM3->CNTIN=0x0000;				  		  				// Free running
	FTM3->MOD=0xFFFF;
	FTM_SetWorkingMode(FTM3,FTM_CH_5,FTM_mInputCapture);   // Select IC Function
	FTM_SetInputCaptureEdge (FTM3, FTM_CH_5,FTM_eEither);  // Capture on both edges
	FTM_SetInterruptMode (FTM3,FTM_CH_5, true);            // Enable interrupts
	FTM_StartClock(FTM3);                                  // Select BusClk
}



// Setters

void FTM_SetPrescaler (FTM_t ftm, FTM_Prescal_t data)
{
	ftm->SC = (ftm->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(data);
}

void FTM_SetModulus (FTM_t ftm, FTMData_t data)
{
	ftm->CNTIN = 0X00;
	ftm->CNT = 0X00;
	ftm->MOD = FTM_MOD_MOD(data);
}

FTMData_t FTM_GetModulus (FTM_t ftm)
{
	return ftm->MOD & FTM_MOD_MOD_MASK;
}

void FTM_StartClock (FTM_t ftm)
{
	ftm->SC |= FTM_SC_CLKS(0x01);
}

void FTM_StopClock (FTM_t ftm)
{
	ftm->SC &= ~FTM_SC_CLKS(0x01);
}

void FTM_SetOverflowMode (FTM_t ftm, bool mode)
{
	ftm->SC = (ftm->SC & ~FTM_SC_TOIE_MASK) | FTM_SC_TOIE(mode);
}

bool FTM_IsOverflowPending (FTM_t ftm)
{
	return ftm->SC & FTM_SC_TOF_MASK;
}

void FTM_ClearOverflowFlag (FTM_t ftm)
{
	ftm->SC &= ~FTM_SC_TOF_MASK;
}

void FTM_SetWorkingMode (FTM_t ftm, FTMChannel_t channel, FTMMode_t mode)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) |
			                      (FTM_CnSC_MSB((mode >> 1) & 0X01) | FTM_CnSC_MSA((mode >> 0) & 0X01));
}

FTMMode_t FTM_GetWorkingMode (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) >> FTM_CnSC_MSA_SHIFT;
}

void FTM_SetInputCaptureEdge (FTM_t ftm, FTMChannel_t channel, FTMEdge_t edge)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
				                  (FTM_CnSC_ELSB((edge >> 1) & 0X01) | FTM_CnSC_ELSA((edge >> 0) & 0X01));
}

FTMEdge_t FTM_GetInputCaptureEdge (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetOutputCompareEffect (FTM_t ftm, FTMChannel_t channel, FTMEffect_t effect)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
				                  (FTM_CnSC_ELSB((effect >> 1) & 0X01) | FTM_CnSC_ELSA((effect >> 0) & 0X01));
}

FTMEffect_t FTM_GetOutputCompareEffect (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetPulseWidthModulationLogic (FTM_t ftm, FTMChannel_t channel, FTMLogic_t logic)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
				                  (FTM_CnSC_ELSB((logic >> 1) & 0X01) | FTM_CnSC_ELSA((logic >> 0) & 0X01));
}

FTMLogic_t FTM_GetPulseWidthModulationLogic (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetCounter (FTM_t ftm, FTMChannel_t channel, FTMData_t data)
{
	ftm->CONTROLS[channel].CnV = FTM_CnV_VAL(data);
}

FTMData_t FTM_GetCounter (FTM_t ftm, FTMChannel_t channel)
{
	return ftm->CONTROLS[channel].CnV & FTM_CnV_VAL_MASK;
}

void FTM_SetInterruptMode (FTM_t ftm, FTMChannel_t channel, bool mode)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~FTM_CnSC_CHIE_MASK) | FTM_CnSC_CHIE(mode);
}

bool FTM_IsInterruptPending (FTM_t ftm, FTMChannel_t channel)
{
	return ftm->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK;
}

void FTM_ClearInterruptFlag (FTM_t ftm, FTMChannel_t channel)
{
	ftm->CONTROLS[channel].CnSC &= ~FTM_CnSC_CHF_MASK;
}

