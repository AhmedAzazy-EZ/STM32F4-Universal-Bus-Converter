/*
 * Debug.h
 *
 * 	Created: 5-July-23
 *  Author: Ahmed Azazy
 */
 
 #ifndef _DEBUG_H_
 #define _DEBUG_H_
 
#include <stm32f446xx.h>
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define DEBUG_ENABLED 1
	 
	 
	 
	 
	 
#define DEBUG_UART_CONTROLLER ((USART_TypeDef *)(UART5))
#define DEBUG_PORT ((GPIO_TypeDef *)(GPIOC)) 	 
#define DEBUG_UART_ENABLE_CLOCK __UART5_CLK_ENABLE
#define DEBUG_PORT_ENABLE_CLOCK __GPIOA_CLK_ENABLE
#define DEBUX_TX_PIN_NUMBER ((int)(12))
#define DEBUG_RX_PIN_NUMBER ((int)(13))




void Debug_Init(void);
void print_DebugMsg(char * str);

 
 #ifdef __cplusplus
 }
#endif
#endif