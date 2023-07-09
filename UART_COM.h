/*
 * UART_COM.h
 *
 * 	Created: 9-July-23
 *  Author: Ahmed Azazy
 */
 
 
#ifndef _UART_COM_H_
#define _UART_COM_H_

#include "COM_Generic.h"

#ifdef __cplusplus
 extern "C" {
#endif	 
	 
class UART_COM : public BASE_COM
{
	
public:
	UART_COM(uint32_t baud_rate , USART_TypeDef * uart_contr , GPIO_TypeDef * GPIO_contr , uint32_t Tx_pinNum , uint32_t Rx_pinNum , uint8_t AF);
	~UART_COM();
private :
	void uart_low_level_init(GPIO_TypeDef * GPIO_contr);
  GPIO_TypeDef * GPIO_Controller;
	UART_HandleTypeDef * uart_handler;
	uint32_t Tx_pinNumber;
	uint32_t Rx_pinNumber; 
  //User has to get this value from the datasheet of the target
	uint8_t Alternate_Function; 
	
	
};
	 
	 
	
	 
	 
#ifdef __cplusplus
 }
#endif
#endif