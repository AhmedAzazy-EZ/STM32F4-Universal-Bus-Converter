#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stm32f446xx.h>
#include "stm32f4xx_hal.h"
#include "BASE_COM.h"
#include "Debug.h"


char generic_buffer[500];
int generic_int;

int main(void)
{
		
	HAL_Init();
	HAL_InitTick(0); 
	Debug_Init();
	
	while(1)
	{
		generic_int++;
		memset(generic_buffer , 0 , 500);
		sprintf(generic_buffer , "Yoooo %d\n" , generic_int);
		print_DebugMsg(generic_buffer);	
		HAL_Delay(1000);
	}

}