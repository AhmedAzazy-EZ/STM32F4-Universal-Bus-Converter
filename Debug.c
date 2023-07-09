/*
 * Debug.c
 *
 * 	Created: 5-July-23
 *  Author: Ahmed Azazy
 */
 
#include "Debug.h"
#include "stm32f4xx_hal.h"



UART_HandleTypeDef Debug_uart;
int dummy;
void Debug_Init(void)
{
	DEBUG_UART_ENABLE_CLOCK();
	DEBUG_PORT_ENABLE_CLOCK();
	
	GPIO_InitTypeDef _GPIO = {0};
	
	
	_GPIO.Pin=	(1<<DEBUX_TX_PIN_NUMBER);
	_GPIO.Mode = GPIO_MODE_AF_PP  ;
	_GPIO.Pull = GPIO_NOPULL;
	_GPIO.Alternate = GPIO_AF8_UART4;
	_GPIO.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(DEBUG_PORT , &_GPIO);


	Debug_uart.Instance = DEBUG_UART_CONTROLLER;
	Debug_uart.Init.BaudRate = 115200;
	Debug_uart.Init.WordLength = UART_WORDLENGTH_8B;
	Debug_uart.Init.StopBits = USART_STOPBITS_1;
	Debug_uart.Init.Parity = USART_PARITY_NONE ;
	Debug_uart.Init.Mode = USART_MODE_TX;
	NVIC_EnableIRQ(UART4_IRQn);
	HAL_UART_Init(&Debug_uart);	
}

void print_DebugMsg(char * str)
{
	#if DEBUG_ENABLED == 1
	if(Debug_uart.gState != HAL_UART_STATE_BUSY_TX){
		HAL_UART_Transmit_IT(&Debug_uart , (uint8_t *) str , strlen(str));
	}
	#else
	
	
	#endif	
}


void SysTick_Handler()
{
	HAL_IncTick();
}

void UART4_IRQHandler()
{
	HAL_UART_IRQHandler(&Debug_uart);
}