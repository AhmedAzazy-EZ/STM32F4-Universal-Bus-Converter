/*
 * UART_COM.h
 *
 * 	Created: 9-July-23
 *  Author: Ahmed Azazy
 */
 
 
#ifndef _UART_COM_H_
#define _UART_COM_H_
	 
#include "BASE_COM.h"
	 
class UART_COM : public BASE_COM_t
{
	
public:
	UART_COM(uint32_t baud_rate , USART_TypeDef * uart_contr , GPIO_TypeDef * GPIO_contr , uint32_t Tx_pinNum , uint32_t Rx_pinNum , uint8_t AF);
	~UART_COM();
	UART_HandleTypeDef * Get_UART_HandleTypeDef();
	STD_Return_t Send(char * data , uint32_t len) override;
	STD_Return_t Receive()override;
	void Receive_callback()override;
  void Send_callback() override;
	void Interrupt_handler()override;
private :
	void uart_low_level_init(GPIO_TypeDef * GPIO_contr);
	UART_HandleTypeDef * uart_handler;
};
	 
	 
	
	 
	 

#endif