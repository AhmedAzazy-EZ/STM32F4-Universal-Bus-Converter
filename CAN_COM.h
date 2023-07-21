/*
 * CAN_COM.h
 *
 * 	Created: 21-July-23
 *  Author: Ahmed Azazy
 */
 
 #ifndef _CAN_COM_H_
 #define _CAN_COM_H_
 
 #include "BASE_COM.h"
 
 
 
class CAN_COM : public BASE_COM_t
{
 
public :
	CAN_COM();
	~CAN_COM();
	STD_Return_t Send(char * data , uint32_t len) override;
	STD_Return_t Receive()override;
	void Receive_callback()override;
  void Send_callback() override;
	void Interrupt_handler()override;

private :
	CAN_InitTypeDef * CAN_handler;
	void CAN_low_level_init(GPIO_TypeDef * GPIO_contr);
 
}
 
 
 
 
 #endif