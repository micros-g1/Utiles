
#include "GPIO.h"

void GPIO_Init (void)
{
	/*
	 * TO DO
	 */
}

//void GPIO_SetDirection (GPIO_t gpio, GPIOMask_t mask, GPIODirection_t direction)
//{
//	gpio->PDDR = (gpio->PDDR & ~mask) | (direction & mask);
//}

void GPIO_SetDirection (GPIO_t gpio, GPIOMask_t pin, GPIODirection_t direction)
{
	gpio->PDDR = (gpio->PDDR & ~(1<<pin) ) | (direction & (1 << pin));
}

GPIODirection_t GPIO_GetDirection (GPIO_t gpio, GPIOMask_t mask)
{
	return gpio->PDDR & mask;
}

//void GPIO_Write (GPIO_t gpio, GPIOMask_t mask, GPIOData_t data)
//{
//	gpio->PDOR = (gpio->PDOR & ~mask) | (data & mask);
//}

void GPIO_Write (GPIO_t gpio, GPIOMask_t pin, GPIOData_t data)
{
	gpio->PDOR = (gpio->PDOR & ~(1<<pin)) | (data & (1 << pin));
}

//GPIOData_t GPIO_Read (GPIO_t gpio, GPIOMask_t mask)
//{
//	return gpio->PDIR & mask;
//}

GPIOData_t GPIO_Read (GPIO_t gpio, GPIOMask_t pin)
{
	return gpio->PDIR & (1<<pin);
}




void GPIO_Set (GPIO_t gpio, GPIOMask_t mask)
{
	gpio->PSOR = mask;
}

void GPIO_Clear	(GPIO_t gpio, GPIOMask_t mask)
{
	gpio->PCOR = mask;
}

void GPIO_Toggle (GPIO_t gpio, GPIOMask_t mask)
{
	gpio->PTOR = mask;
}
