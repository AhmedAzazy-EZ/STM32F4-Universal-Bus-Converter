/*
 * UART_COM.cpp
 *
 * 	Created: 9-July-23
 *  Author: Ahmed Azazy
 */

#include "COM_Generic.h"

UART_COM::UART_COM(uint32_t baud_rate , USART_TypeDef * uart_contr , GPIO_TypeDef * GPIO_contr , uint32_t Tx_pinNum , uint32_t Rx_pinNum , uint8_t AF)
{
	
	UART_COM::uart_low_level_init(GPIO_contr);
	GPIO_InitTypeDef _GPIO = {0};
	_GPIO.Pin=	(1<<Tx_pinNum) | (1<<Rx_pinNum);
	_GPIO.Mode = GPIO_MODE_AF_PP  ;
	_GPIO.Pull = GPIO_NOPULL;
	_GPIO.Alternate = AF;
	_GPIO.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIO_contr , &_GPIO);
	
	uart_handler = new UART_HandleTypeDef;
	memset(uart_handler, 0, sizeof(UART_HandleTypeDef));
	uart_handler->Instance = uart_contr;
	uart_handler->Init.BaudRate = baud_rate;
	uart_handler->Init.WordLength = UART_WORDLENGTH_8B;
	uart_handler->Init.StopBits = USART_STOPBITS_1;
	uart_handler->Init.Parity = USART_PARITY_NONE;
	uart_handler->Init.Mode = USART_MODE_TX | USART_MODE_RX;
	HAL_UART_Init(uart_handler);	
	
}

UART_COM::~UART_COM()
{
	delete uart_handler;
}

void UART_COM::uart_low_level_init(GPIO_TypeDef * GPIO_contr)
{
	
	UART_COM::GPIO_clock_Enable(GPIO_contr);
	switch((uint32_t)uart_handler->Instance)
	{
	
		case ((uint32_t) USART1_BASE):
			__USART1_CLK_ENABLE();
			NVIC_EnableIRQ(USART1_IRQn);
			break;
		case ((uint32_t) USART2_BASE):
			__USART2_CLK_ENABLE();
			NVIC_EnableIRQ(USART2_IRQn);
			break;
		case ((uint32_t) USART3_BASE):
			__USART3_CLK_ENABLE();
			NVIC_EnableIRQ(USART3_IRQn);
			break;			
		case ((uint32_t) UART4_BASE):
			__UART4_CLK_ENABLE();
			NVIC_EnableIRQ(UART4_IRQn);
			break;
		case ((uint32_t) UART5_BASE):
			__UART5_CLK_ENABLE();
			NVIC_EnableIRQ(UART5_IRQn);
			break;		
		case ((uint32_t) USART6_BASE):
			__USART6_CLK_ENABLE();
			NVIC_EnableIRQ(USART6_IRQn);
			break;			
	}
	
}