/*
 * BASE_COM.h
 *
 * 	Created: 5-July-23
 *  Author: Ahmed Azazy
 */

#pragma once

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "COM_Generic.h"

class BASE_COM
{
public :
	virtual BASE_COM() = 0;
	virtual Send() = 0;
	virtual Receive() = 0;
	virtual Init() = 0;
	virtual Deinit() = 0;

	void Pin_setter(GPIO_TypeDef  * Myport , uint32_t My_pin_Number);
	uint8_t pin_read();
	
protected:
	uint32_t pin_Number;
	GPIO_TypeDef  *port;
};


#ifdef __cplusplus
 }
 
#endif