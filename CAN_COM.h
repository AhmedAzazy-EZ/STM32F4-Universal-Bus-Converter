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
	CAN_COM(CAN_TypeDef * can_contr , GPIO_TypeDef * GPIO_contr , uint8_t tx_pin , uint8_t rx_pin , uint16_t Receive_ID , uint16_t Send_ID);
	~CAN_COM();
	STD_Return_t Send(char * data , uint32_t len) override;
	STD_Return_t Receive()override;
	CAN_HandleTypeDef * Get_CAN_HandleTypeDef();
	void Receive_callback()override;
  void Send_callback() override;
	void Interrupt_handler()override;
	void Receive_FIFO0_cb(); // CAN_RX_FIFO0
	void Receive_FIFO1_cb(); // CAN_RX_FIFO1


private :
	CAN_HandleTypeDef * CAN_handler;
	void CAN_low_level_init(GPIO_TypeDef * GPIO_contr);
	CAN_RxHeaderTypeDef FIFO0_Rx_header;
	CAN_RxHeaderTypeDef FIFO1_Rx_header;
 
};
 
 
 
 
 #endif