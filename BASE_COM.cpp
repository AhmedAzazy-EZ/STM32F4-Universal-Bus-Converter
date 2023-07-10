/*
 * BASE_COM.cpp
 *
 * 	Created: 5-July-23
 *  Author: Ahmed Azazy
 */

#include "COM_Generic.h"

BASE_COM_t::BASE_COM_t()
{

}

void BASE_COM_t::Pin_setter(GPIO_TypeDef  * Myport , uint32_t My_pin_Number)
{
	port = Myport;
	pin_Number = My_pin_Number;
}


uint8_t BASE_COM_t::pin_read()
{
	return (uint8_t)HAL_GPIO_ReadPin(port, pin_Number);
}


void BASE_COM_t::GPIO_clock_Enable(GPIO_TypeDef  * Myport)
{
	
	switch((uint32_t)Myport)
	{
		case (uint32_t)GPIOA_BASE:
			__GPIOA_CLK_ENABLE();
			break;
		case (uint32_t)GPIOB_BASE:
			__GPIOB_CLK_ENABLE();
			break;
		case (uint32_t)GPIOC_BASE:
			__GPIOC_CLK_ENABLE();
			break;
		case (uint32_t)GPIOD_BASE:
			__GPIOD_CLK_ENABLE();
			break;
		case (uint32_t)GPIOE_BASE:
			__GPIOE_CLK_ENABLE();
			break;
		case (uint32_t)GPIOF_BASE:
			__GPIOF_CLK_ENABLE();
			break;
		case (uint32_t)GPIOG_BASE:
			__GPIOG_CLK_ENABLE();
			break;
	}

}


BASE_COM_t::~BASE_COM_t()
{

}