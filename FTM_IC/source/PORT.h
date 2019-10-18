
#ifndef SOURCES_PORT_H_
#define SOURCES_PORT_H_

#include "hardware.h"
/*
 * See _BASE_PTRS and _IRQS
 */


typedef struct{

	 uint32_t PS		:1;
	 uint32_t PE		:1;
	 uint32_t SRE		:1;
	 uint32_t RESERV0	:1;
	 uint32_t PFE		:1;
	 uint32_t ODE		:1;
	 uint32_t DSE		:1;
	 uint32_t RESERV1	:1;
	 uint32_t MUX		:3;
	 uint32_t RESERV2	:4;
	 uint32_t LK		:1;
	 uint32_t IRQC		:4;
	 uint32_t RESERV3	:4;
	 uint32_t ISF		:1;
	 uint32_t RESERV4	:7;


}FIELD_T;  //PCR Structure


typedef union

{
	FIELD_T    FIELD;
	uint32_t   PCR;

}PCRstr;   //




typedef enum
{
	PORT_mAnalog,
	PORT_mGPIO,
	PORT_mAlt2,
	PORT_mAlt3,
	PORT_mAlt4,
	PORT_mAlt5,
	PORT_mAlt6,
	PORT_mAlt7,
} PORTMux_t;

typedef enum
{
	PORT_eDisabled				= 0x00,
	PORT_eDMARising				= 0x01,
	PORT_eDMAFalling			= 0x02,
	PORT_eDMAEither				= 0x03,
	PORT_eInterruptDisasserted	= 0x08,
	PORT_eInterruptRising		= 0x09,
	PORT_eInterruptFalling		= 0x0A,
	PORT_eInterruptEither		= 0x0B,
	PORT_eInterruptAsserted		= 0x0C,
} PORTEvent_t;

typedef PORT_Type *PORT_t; /* PORTA, PORTB, PORTC, PORTD, PORTE */
typedef uint32_t PORTBit_t;
typedef uint32_t PORTFlags_t;

//__ISR__ 	PORTA_IRQHandler			  (void);
//__ISR__ 	PORTB_IRQHandler			  (void);
//__ISR__ 	PORTC_IRQHandler			  (void);
//__ISR__ 	PORTD_IRQHandler			  (void);
//__ISR__ 	PORTE_IRQHandler			  (void);

void	 	PORT_Init				  	  (void);

void 	 	PORT_SetPullResistorMode  	  (PORT_t, PORTBit_t, bool);
void 	 	PORT_SetPullUpMode	   	  	  (PORT_t, PORTBit_t, bool);
void 	 	PORT_SetOpenDrainMode	  	  (PORT_t, PORTBit_t, bool);
void 	 	PORT_SetPassiveFilterMode 	  (PORT_t, PORTBit_t, bool);
void 	 	PORT_SetSlowSlewRateMode  	  (PORT_t, PORTBit_t, bool);
void 	 	PORT_SetHighDriveStrengthMode (PORT_t, PORTBit_t, bool);

void 	 	PORT_SetMuxMode		   	  	  (PORT_t, PORTBit_t, PORTMux_t);

void 	 	PORT_SetInterruptMode	  	  (PORT_t, PORTBit_t, PORTEvent_t);
bool 	 	PORT_IsInterruptPending   	  (PORT_t, PORTBit_t);
void 	 	PORT_ClearInterruptFlag   	  (PORT_t, PORTBit_t);

void 		PORT_Configure		  	 	  (PORT_t, PORTBit_t, bool, bool, bool, bool, bool, bool, PORTMux_t, PORTEvent_t);
void 		PORT_Configure2 			   (PORT_t port, PORTBit_t bit,PCRstr UserPCR);


PORTFlags_t PORT_GetPortFlags	      	  (PORT_t);
void 	 	PORT_ClearPortFlags	      	  (PORT_t);

#endif /* SOURCES_PORT_H_ */
