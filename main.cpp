#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <stm32f446xx.h>
#include "stm32f4xx_hal.h"
#include "BASE_COM.h"
#include "UART_COM.h"
#include "Debug.h"
#include <vector>

//extern std::vector<BASE_COM_t *> COM_ABSTRACT;

char generic_buffer[500];
int generic_int;
	 
UART_COM My_UART4{115200 , UART4 , GPIOA , 0 , 1 , GPIO_AF8_UART4};
UART_COM My_UART5{115200 , UART5 , GPIOC , 12 , 13 ,  GPIO_AF8_UART5};


int main(void)
{
	
	HAL_Init();
	HAL_InitTick(0); 		
	//register observers
	My_UART4.obsrvables_tracking.push_back(My_UART5.attatch(&My_UART4 , &My_UART5));
	My_UART5.obsrvables_tracking.push_back(My_UART4.attatch(&My_UART5 , &My_UART4));
	
	
	while(1)
	{
		My_UART4.poll();
		HAL_Delay(1);
		My_UART5.poll();
	}

}

 #ifdef __cplusplus
 extern "C" {
#endif
	 
void HardFault_Handler (void) 
{
 __NVIC_SystemReset();			
}


void UART4_IRQHandler()
{
	My_UART4.Interrupt_handler();
}

void UART5_IRQHandler()
{
	My_UART5.Interrupt_handler();
}

void SysTick_Handler()
{
	HAL_IncTick();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == My_UART5.Get_UART_HandleTypeDef())
	{
			My_UART5.Receive_callback();
	}
	
	else if(huart == My_UART4.Get_UART_HandleTypeDef())
	{
		My_UART4.Receive_callback();
	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == My_UART4.Get_UART_HandleTypeDef())
	{
		My_UART4.Send_callback();
	}
	
	else if(huart == My_UART5.Get_UART_HandleTypeDef())
	{
		My_UART5.Send_callback();
	}
}
	 
#ifdef __cplusplus
}
#endif


