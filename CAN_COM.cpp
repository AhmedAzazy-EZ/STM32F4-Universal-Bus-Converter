/*
 * CAN_COM.cpp
 *
 * 	Created: 21-July-23
 *  Author: Ahmed Azazy
 */

#include "COM_Generic.h"
#include "CAN_COM.h"
 
CAN_COM::CAN_COM(CAN_TypeDef * can_contr , GPIO_TypeDef * GPIO_contr , uint8_t tx_pin , uint8_t rx_pin , uint16_t My_Receive_ID , uint16_t My_Send_ID , uint8_t AF)
 {
	 CAN_FilterTypeDef sFilterConfig = {0};
	 CAN_handler = new CAN_HandleTypeDef;
	 memset(CAN_handler , 0 , sizeof(CAN_HandleTypeDef));
	 Receive_ID = My_Receive_ID;
	 Send_ID = My_Send_ID;
	 CAN_handler->Instance = can_contr;
	 CAN_low_level_init(GPIO_contr);
	 
	 
	GPIO_InitTypeDef _GPIO = {0};
	_GPIO.Pin=	(1<<tx_pin) | (1<<rx_pin);
	_GPIO.Mode = GPIO_MODE_AF_OD  ;
	_GPIO.Pull = GPIO_NOPULL;
	_GPIO.Alternate = AF;
	_GPIO.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIO_contr , &_GPIO);
	
	 /* * APB Clock Source is 22Mhz 
		  * CAN Prescaler 11 , Seg1 -> 2 tq , seg2 -> 1 tq
			* then CAN Clock Sourec is 2 Mhz
			* Nominal bit time = (1/2Mhz) + 2 * (1/2Mhz) + (1 / 2Mhz) = (1 / 500000) Sec
			* then bit rate = 500000
	 */
	 CAN_handler->Init.Prescaler = 11;
	 CAN_handler->Init.Mode = CAN_MODE_NORMAL;
	 CAN_handler->Init.TimeSeg1 = CAN_BS1_2TQ;
	 CAN_handler->Init.TimeSeg2 = CAN_BS2_1TQ;
	 CAN_handler->Init.TimeTriggeredMode = DISABLE;
	 CAN_handler->Init.AutoBusOff = DISABLE;
	 CAN_handler->Init.AutoWakeUp = DISABLE;
	 CAN_handler->Init.AutoRetransmission = ENABLE;
	 CAN_handler->Init.ReceiveFifoLocked = DISABLE;
	 CAN_handler->Init.TransmitFifoPriority = DISABLE;
	 HAL_CAN_Init(CAN_handler);
	 
	 
	 sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
	 sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	 sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	 sFilterConfig.FilterBank = 0;
	 sFilterConfig.FilterIdHigh = My_Receive_ID << 5; //Shift left 5 bits according to the RM P1057
	 sFilterConfig.FilterIdLow = 0;
	 sFilterConfig.FilterMaskIdHigh = 0;
	 sFilterConfig.FilterMaskIdLow = 0;
	 sFilterConfig.SlaveStartFilterBank = 25; //Just 3 Filters  25:27 are assigned to CAN2
	 sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	 HAL_CAN_ConfigFilter (CAN_handler, &sFilterConfig);
	 
	 
	 HAL_CAN_Start (CAN_handler);
	 HAL_CAN_ActivateNotification(CAN_handler ,(CAN_IT_RX_FIFO0_MSG_PENDING  | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_OVERRUN \
	 | CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO1_FULL | CAN_IT_RX_FIFO1_OVERRUN | CAN_IT_TX_MAILBOX_EMPTY));
	  
 }
 
 
CAN_COM::~CAN_COM()
{
	delete CAN_handler;
	delete this;
}
 
void CAN_COM::CAN_low_level_init(GPIO_TypeDef * GPIO_contr)
{
	GPIO_clock_Enable(GPIO_contr);
	switch((uint32_t)CAN_handler->Instance)
	{
		case (uint32_t)CAN1_BASE:
			__CAN1_CLK_ENABLE();
			NVIC_EnableIRQ(CAN1_TX_IRQn);
			NVIC_EnableIRQ(CAN1_RX0_IRQn); //FIFO0 Receiver
			NVIC_EnableIRQ(CAN1_RX1_IRQn); //FIFO1 Receiver
			NVIC_EnableIRQ(CAN1_SCE_IRQn); //Status Change Error Intterupt
			break;

		case (uint32_t)CAN2_BASE:
			__CAN2_CLK_ENABLE();
			NVIC_EnableIRQ(CAN2_TX_IRQn);
			NVIC_EnableIRQ(CAN2_RX0_IRQn); //FIFO0 Receiver
			NVIC_EnableIRQ(CAN2_RX1_IRQn); //FIFO1 Receiver
			NVIC_EnableIRQ(CAN2_SCE_IRQn); //Status Change Error Intterupt
			break;			
	}

}

STD_Return_t CAN_COM::Send(char * data , uint32_t len)    
{
	
	return true;
}


// CAN Protocol is Event Based So you dont have to define anything here 
//once a message is received the CAN Controller will Notify the Software
//Then proceed with extracting the received message 
//in other words just define The Receive CallBack
STD_Return_t CAN_COM::Receive()
{	
	return true;
}

void CAN_COM::Receive_callback()
{
//	 HAL_CAN_GetRxMessage (CAN_handler , uint32_t RxFifo,
//CAN_RxHeaderTypeDef * pHeader, uint8_t aData);
	
	//(* RxFifo0MsgPendingCallback)(struct __CAN_HandleTypeDef *hcan)
}

void CAN_COM::Receive_FIFO0_cb()
{
	if(HAL_CAN_GetRxMessage (CAN_handler , CAN_RX_FIFO0, &FIFO0_Rx_header, &receive_buffer[receive_tracker%COM_BUFFER_MAX_LENGTH]) == HAL_OK)
	{
		receive_tracker+= FIFO0_Rx_header.DLC;
		Notify_observers(this);
	}
}


void CAN_COM::Receive_FIFO1_cb()
{
	if(HAL_CAN_GetRxMessage (CAN_handler , CAN_RX_FIFO1, &FIFO1_Rx_header, &receive_buffer[receive_tracker%COM_BUFFER_MAX_LENGTH]) == HAL_OK)
	{
		receive_tracker+= FIFO1_Rx_header.DLC;
		Notify_observers(this);
	}
}

void CAN_COM::Send_callback()
{
	
}

void CAN_COM::Interrupt_handler()
{
	HAL_CAN_IRQHandler (CAN_handler);
}

