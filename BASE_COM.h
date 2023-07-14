/*
 * BASE_COM.h
 *
 * 	Created: 5-July-23
 *  Author: Ahmed Azazy
 */

#ifndef _BASE_COM_H_
#define _BASE_COM_H_

extern "C" {	 
#include "std_types.h"
#include "stm32f4xx_hal.h"
}

#include "observer_pattern.h"

class BASE_COM_t : public observer , public subject 
{
public :
	BASE_COM_t();
	~BASE_COM_t();
	virtual STD_Return_t Send(char * data , uint32_t len) = 0;
	virtual STD_Return_t Receive() = 0;
	virtual void Receive_callback() = 0;
	virtual void Send_callback() = 0;
	virtual void Interrupt_handler()=0;
	virtual void poll();
	void Pin_setter(GPIO_TypeDef  * Myport , uint32_t My_pin_Number);
	uint8_t pin_read();
	void GPIO_clock_Enable(GPIO_TypeDef  * Myport);
	uint32_t receive_tracker = 0;
	static int Num_of_instances;
	uint8_t receive_buffer[500];
protected:
	uint32_t pin_Number;
	GPIO_TypeDef  *port;
	bool OnGoingTx;
	
};

 
#endif