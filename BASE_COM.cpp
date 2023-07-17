/*
 * BASE_COM.cpp
 *
 * 	Created: 5-July-23
 *  Author: Ahmed Azazy
 */

#include "COM_Generic.h"
#include <vector>

using namespace std;

vector<BASE_COM_t *> COM_ABSTRACT;
int BASE_COM_t::Num_of_instances = 0;

BASE_COM_t::BASE_COM_t()
{
	COM_ABSTRACT.push_back(this);
	Num_of_instances++;	
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


void BASE_COM_t::poll()
{
	if(!OnGoingTx)
	{
		for(send_handle * item : obsrvables_tracking)
		{
			if(item->data_to_be_sent_tracker < item->data_received_tracker)
			{
				Send((char *)&item->Source_COM->receive_buffer[(item->data_to_be_sent_tracker++)%COM_BUFFER_MAX_LENGTH] , 1);
				return;
			}
		}
	}
}

BASE_COM_t::~BASE_COM_t()
{

}