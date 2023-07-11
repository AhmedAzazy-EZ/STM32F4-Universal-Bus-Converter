/*
 * BASE_COM.h
 *
 * 	Created: 5-July-23
 *  Author: Ahmed Azazy
 */

#ifndef _BASE_COM_H_
#define _BASE_COM_H_

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "std_types.h"
	 
class BASE_COM_t
{
public :
	BASE_COM_t();
	~BASE_COM_t();
	virtual STD_Return_t Send(char * data , uint32_t len) = 0;
	virtual STD_Return_t Receive(char * user_buff , uint32_t len) = 0;

	void Pin_setter(GPIO_TypeDef  * Myport , uint32_t My_pin_Number);
	uint8_t pin_read();
	void GPIO_clock_Enable(GPIO_TypeDef  * Myport);
	static int Num_of_instances;
protected:
	uint32_t pin_Number;
	GPIO_TypeDef  *port;
};




#ifdef __cplusplus
 }
 
#endif
#endif