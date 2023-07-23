/*
 * CAN_COM.cpp
 *
 * 	Created: 21-July-23
 *  Author: Ahmed Azazy
 */
 
#include "CAN_COM.h"
 
CAN_COM::	CAN_COM(CAN_TypeDef * can_contr , GPIO_TypeDef * GPIO_contr , uint8_t tx_pin , uint8_t rx_pin , uint16_t Receive_ID , uint16_t Send_ID)
 {
	 CAN_handler = new CAN_HandleTypeDef;
	 memset(CAN_handler , 0 , sizeof(CAN_HandleTypeDef));
	 CAN_handler->Instance = can_contr;
	 CAN_low_level_init(GPIO_contr);
	 
	 /* *APB Clock Source is 22Mhz 
		  *CAN Prescaler 11 , Seg1 -> 2 tq , seg2 -> 1 tq
			*then CAN Clock Sourec is 2 Mhz
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
	 HAL_CAN_ActivateNotification(CAN_handler ,(CAN_IT_RX_FIFO0_MSG_PENDING  | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_OVERRUN \
	 | CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO1_FULL | CAN_IT_RX_FIFO1_OVERRUN | CAN_IT_TX_MAILBOX_EMPTY));
	 HAL_CAN_Start (CAN_handler);
	 
	 
//	 HAL_CAN_ConfigFilter (CAN_HandleTypeDef * hcan, const CAN_FilterTypeDef *
//sFilterConfig);
//	 Receive();
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
}

void CAN_COM::Send_callback()
{
	
}

void CAN_COM::Interrupt_handler()
{
	HAL_CAN_IRQHandler (CAN_handler);
}

