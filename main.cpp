#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <stm32f446xx.h>
#include "stm32f4xx_hal.h"
#include "Debug.h"
#include <vector>
#include "BASE_COM.h"
#include "UART_COM.h"
#include "I2C_COM.h"
#include "CAN_COM.h"




/*************Generic Variables***************/
char generic_buffer[500];
int generic_int;
I2C_HandleTypeDef i2c2;
bool I2C_send_trigger;

CAN_FilterTypeDef Filter = {0};
CAN_HandleTypeDef can2;
bool CAN_send_trigger;


/************Private functions****************/
static void I2C2_Slave_init(void);
static void CAN2_init();
static void Clock_Config(void);
static void I2C_obj_trigger(void);
static void CAN_obj_trigger(void);






/***************COMM Ports********************/
UART_COM * My_UART4 = nullptr;
UART_COM * My_UART5 = nullptr;
I2C_COM * My_I2C1 = nullptr;
CAN_COM * My_CAN1 = nullptr;

int main(void)
{
	
	HAL_Init();
	HAL_InitTick(0); 	
	Clock_Config(); //Setting the Core processor to 180 Mhz	
	I2C2_Slave_init();

	
	UART_COM My_UART4_obj{115200 , UART4 , GPIOA , 0 , 1 , GPIO_AF8_UART4};
	UART_COM My_UART5_obj{115200 , UART5 , GPIOC , 12 , 13 ,  GPIO_AF8_UART5};
	I2C_COM My_I2C1_obj{100000 , 0x25 , 0x26 , I2C1 , GPIOB , 6 , 7 , 4};
	CAN_COM My_CAN1_obj{CAN1 , GPIOA , 12 , 11 , 8 , 9 , 9};
	My_UART4 = &My_UART4_obj;
	My_UART5 = &My_UART5_obj;
	My_I2C1 = &My_I2C1_obj;
	My_CAN1 = &My_CAN1_obj;
	//register observers
	My_UART4->obsrvables_tracking.push_back(My_UART5->attatch(My_UART4 , My_UART5)); //Attach UART4 to UART5
	My_UART5->obsrvables_tracking.push_back(My_UART4->attatch(My_UART5 , My_UART4)); //Attach UART5 to UART4 
	My_I2C1->obsrvables_tracking.push_back(My_UART4->attatch(My_I2C1 , My_UART4));   //Attatch I2C1 to UART4
	My_UART4->obsrvables_tracking.push_back(My_I2C1->attatch(My_UART4 , My_I2C1));    //attach UART4 to I2C1
	
	
	CAN2_init();
	while(1)
	{
		My_UART4->poll();
		HAL_Delay(1);
		My_UART5->poll();
		HAL_Delay(1);
		My_I2C1->poll();
		
		
		I2C_obj_trigger(); //I2C2 Sends Data to My_I2C1 
		CAN_obj_trigger(); //CAN2 Sends Data to My_CAN1
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
	My_I2C1->Interrupt_handler();
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c == My_I2C1->Get_I2c_Handle_TypeDef())
	{
		My_I2C1->Send_callback();
	}
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c == My_I2C1->Get_I2c_Handle_TypeDef())
	{
		My_I2C1->Receive_callback();
	}
}

/************************End of I2C Handlers*********************************/


/************************UART Handlers*********************************/

void UART4_IRQHandler()
{
	My_UART4->Interrupt_handler();
}

void UART5_IRQHandler()
{
	My_UART5->Interrupt_handler();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == My_UART5->Get_UART_HandleTypeDef())
	{
			My_UART5->Receive_callback();
	}
	
	else if(huart == My_UART4->Get_UART_HandleTypeDef())
	{
		My_UART4->Receive_callback();
	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == My_UART4->Get_UART_HandleTypeDef())
	{
		My_UART4->Send_callback();
	}
	
	else if(huart == My_UART5->Get_UART_HandleTypeDef())
	{
		My_UART5->Send_callback();
	}
}
/************************End of UART Handlers*********************************/


/************************CAN Handlers*********************************/

void CAN1_TX_IRQHandler()
{
	My_CAN1->Interrupt_handler();
}

void CAN1_RX0_IRQHandler()
{
	My_CAN1->Interrupt_handler();
}

void CAN1_RX1_IRQHandler()
{
	My_CAN1->Interrupt_handler();
}

void CAN1_SCE_IRQHandler()
{
	My_CAN1->Interrupt_handler();
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan == My_CAN1->Get_CAN_HandleTypeDef())
	{
		My_CAN1->Receive_FIFO0_cb();
	}	
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan == My_CAN1->Get_CAN_HandleTypeDef())
	{
		My_CAN1->Receive_FIFO1_cb();
	}	
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan == My_CAN1->Get_CAN_HandleTypeDef())
	{
		My_CAN1->Send_Mailbox0_Complete_cp();
	}
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan == My_CAN1->Get_CAN_HandleTypeDef())
	{
		My_CAN1->Send_Mailbox1_Complete_cp();
	}	
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan == My_CAN1->Get_CAN_HandleTypeDef())
	{
		My_CAN1->Send_Mailbox2_Complete_cp();
	}	
}


/************************End of CAN Handlers*********************************/

void SysTick_Handler()
{
	HAL_IncTick();
	if(HAL_GetTick() % 1000 == 0)
	{
		I2C_send_trigger = true;
	}
	
	if(HAL_GetTick() % 1500 == 0)
	{
		CAN_send_trigger = true;
	}	
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_CFGR_PPRE1_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_CFGR_PPRE1_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    //Error_Handler();
  }
	SystemCoreClockUpdate();	
}

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

static void I2C_obj_trigger(void)
{
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

void I2C2_EV_IRQHandler()
{
	HAL_I2C_EV_IRQHandler(&i2c2);
}

static void CAN2_init()
{ 
	//PB12 , 13 Rx , Tx
	 CAN_FilterTypeDef sFilterConfig = {0};
	 can2.Instance = CAN2;
	 __GPIOB_CLK_ENABLE();
	__CAN2_CLK_ENABLE();
	NVIC_EnableIRQ(CAN2_TX_IRQn);
	NVIC_EnableIRQ(CAN2_RX0_IRQn); //FIFO0 Receiver
	NVIC_EnableIRQ(CAN2_RX1_IRQn); //FIFO1 Receiver
	NVIC_EnableIRQ(CAN2_SCE_IRQn); //Status Change Error Intterupt	 
	 
	 
	GPIO_InitTypeDef _GPIO = {0};
	_GPIO.Pin=	(1<<12) | (1<<13);
	_GPIO.Mode = GPIO_MODE_AF_PP  ;
	_GPIO.Pull = GPIO_NOPULL;
	_GPIO.Alternate = GPIO_AF9_CAN2;
	_GPIO.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB , &_GPIO);
	
	 /* * APB Clock Source is 22Mhz 
		  * CAN Prescaler 11 , Seg1 -> 2 tq , seg2 -> 1 tq
			* then CAN Clock Sourec is 2 Mhz
			* Nominal bit time = (1/2Mhz) + 2 * (1/2Mhz) + (1 / 2Mhz) = (1 / 500000) Sec
			* then bit rate = 500000
	 */
	 can2.Init.Prescaler = 11;
	 can2.Init.Mode = CAN_MODE_NORMAL;
	 can2.Init.TimeSeg1 = CAN_BS1_2TQ;
	 can2.Init.TimeSeg2 = CAN_BS2_2TQ;
	 can2.Init.TimeTriggeredMode = DISABLE;
	 can2.Init.AutoBusOff = DISABLE;
	 can2.Init.AutoWakeUp = DISABLE;
	 can2.Init.AutoRetransmission = DISABLE;
	 can2.Init.ReceiveFifoLocked = DISABLE;
	 can2.Init.TransmitFifoPriority = DISABLE;
	 HAL_CAN_Init(&can2);
	 
	 
	 sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
	 sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	 sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	 sFilterConfig.FilterBank = 26;
	 sFilterConfig.FilterIdHigh = 9 << 5; //Shift left 5 bits according to the RM P1057
	 sFilterConfig.FilterIdLow = 0 ;
	 sFilterConfig.FilterMaskIdHigh = 0 ;
	 sFilterConfig.FilterMaskIdLow = 0;
	 sFilterConfig.SlaveStartFilterBank = 25; //Just 3 Filters  25:27 are assigned to CAN2
	 sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	 HAL_CAN_ConfigFilter (&can2, &sFilterConfig);
	 
	 
	 HAL_CAN_Start (&can2);
	 HAL_CAN_ActivateNotification(&can2 ,(CAN_IT_RX_FIFO0_MSG_PENDING  | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_OVERRUN \
	 | CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO1_FULL | CAN_IT_RX_FIFO1_OVERRUN | CAN_IT_TX_MAILBOX_EMPTY));
	
}

static void CAN_obj_trigger(void)
{
	if(CAN_send_trigger == true)
	{
		uint8_t data = 0x55;
		CAN_TxHeaderTypeDef My_TxHeader = {0};
		uint32_t MailBox_Num = 0;
		My_TxHeader.DLC = 1;
		My_TxHeader.StdId = 8;
		My_TxHeader.ExtId = 0;
		My_TxHeader.IDE = CAN_ID_STD;
		My_TxHeader.RTR = CAN_RTR_DATA;
		My_TxHeader.TransmitGlobalTime = DISABLE;
		HAL_CAN_AddTxMessage (&can2 , &My_TxHeader, &data , &MailBox_Num);
		CAN_send_trigger = false;
	}	
}

void CAN2_TX_IRQHandler()
{
	HAL_CAN_IRQHandler (&can2);
}

void CAN2_RX0_IRQHandler()
{
	HAL_CAN_IRQHandler (&can2);
}

void CAN2_RX1_IRQHandler()
{
	HAL_CAN_IRQHandler (&can2);
}

void CAN2_SCE_IRQHandler()
{
	HAL_CAN_IRQHandler (&can2);
}


#ifdef __cplusplus
}
#endif


