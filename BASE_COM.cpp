/*
 * BASE_COM.cpp
 *
 * 	Created: 5-July-23
 *  Author: Ahmed Azazy
 */

#include "BASE_COM.h"


void BASE_COM::Pin_setter(GPIO_TypeDef  * Myport , uint32_t My_pin_Number)
{
	port = Myport;
	pin_Number = My_pin_Number;
}


uint8_t BASE_COM::pin_read()
{
	return (uint8_t)HAL_GPIO_ReadPin(port, pin_Number);
}
