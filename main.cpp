#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <stm32f446xx.h>
#include "stm32f4xx_hal.h"
#include "BASE_COM.h"
#include "UART_COM.h"
#include "I2C_COM.h"
#include "Debug.h"
#include <vector>

//extern std::vector<BASE_COM_t *> COM_ABSTRACT;


/*************Generic Variables***************/
char generic_buffer[500];
int generic_int;
I2C_HandleTypeDef i2c2;
bool I2C_send_trigger;



/************Private functions****************/
static void I2C2_Slave_init(void);
static void Clock_Config(void);


/***************COMM Ports********************/
UART_COM My_UART4{115200 , UART4 , GPIOA , 0 , 1 , GPIO_AF8_UART4};
UART_COM My_UART5{115200 , UART5 , GPIOC , 12 , 13 ,  GPIO_AF8_UART5};
I2C_COM My_I2C1{100000 , 0x25 , 0x26 , I2C1 , GPIOB , 6 , 7 , 4};


int main(void)
{
	
	HAL_Init();
	HAL_InitTick(0); 	
	I2C2_Slave_init();
	Clock_Config();
	
	//register observers
	My_UART4.obsrvables_tracking.push_back(My_UART5.attatch(&My_UART4 , &My_UART5)); //Attach UART4 to UART5
	My_UART5.obsrvables_tracking.push_back(My_UART4.attatch(&My_UART5 , &My_UART4)); //Attach UART5 to UART4 
	My_I2C1.obsrvables_tracking.push_back(My_UART4.attatch(&My_I2C1 , &My_UART4));   //Attatch I2C1 to UART4
	My_UART4.obsrvables_tracking.push_back(My_I2C1.attatch(&My_UART4 , &My_I2C1));    //attach UART4 to I2C1
	
	while(1)
	{
		My_UART4.poll();
		HAL_Delay(1);
		My_UART5.poll();
		HAL_Delay(1);
		My_I2C1.poll();
		
		
		
		if(I2C_send_trigger == true)
		{
			memset(generic_buffer , 0 , 500);
			sprintf(generic_buffer , "Ahmed: %d\r\n" , generic_int++);
			for(int i = 0 ; i < strlen(generic_buffer) ; i++)
			{
				HAL_I2C_Master_Transmit(&i2c2 , 0x25 , (uint8_t *)&generic_buffer[i] , 1 , HAL_MAX_DELAY);	
			}
			I2C_send_trigger = false;
		}
	}

}

 #ifdef __cplusplus
 extern "C" {
#endif
	 
void HardFault_Handler (void) 
{
 __NVIC_SystemReset();			
}

/************************I2C Handlers*********************************/

void I2C1_EV_IRQHandler()
{
	My_I2C1.Interrupt_handler();
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c == My_I2C1.Get_I2c_Handle_TypeDef())
	{
		My_I2C1.Send_callback();
	}
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c == My_I2C1.Get_I2c_Handle_TypeDef())
	{
		My_I2C1.Receive_callback();
	}
}

/************************End of I2C Handlers*********************************/


/************************UART Handlers*********************************/

void UART4_IRQHandler()
{
	My_UART4.Interrupt_handler();
}

void UART5_IRQHandler()
{
	My_UART5.Interrupt_handler();
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
/************************End of UART Handlers*********************************/



static void I2C2_Slave_init(void)
{
	GPIO_InitTypeDef GPIO;
	
	__GPIOB_CLK_ENABLE();
	__I2C2_CLK_ENABLE();
	
	GPIO.Pin = GPIO_PIN_3;
	GPIO.Mode = GPIO_MODE_AF_OD;
	GPIO.Pull = GPIO_PULLUP;
	GPIO.Alternate = GPIO_AF4_I2C1;
	GPIO.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB , &GPIO);
	
	GPIO.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOB , &GPIO);	
	
	i2c2.Instance = I2C2;
	i2c2.Init.ClockSpeed = 100000;
	i2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	i2c2.Init.OwnAddress1 = 2;
	i2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c2.Init.OwnAddress2 = 69;
	i2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	NVIC_EnableIRQ(I2C2_EV_IRQn);
	if(HAL_I2C_Init(&i2c2) != HAL_OK)
	{
		while(1);
	}
}

void SysTick_Handler()
{
	HAL_IncTick();
	if(HAL_GetTick() % 1000 == 0)
	{
		I2C_send_trigger = true;
	}
}

void I2C2_EV_IRQHandler()
{
	HAL_I2C_EV_IRQHandler(&i2c2);
}

static void Clock_Config(void)
{	
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    //Error_Handler();
  }
	SystemCoreClockUpdate();	
}

#ifdef __cplusplus
}
#endif


