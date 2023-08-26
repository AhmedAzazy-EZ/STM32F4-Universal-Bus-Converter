#include "ETHERNET_COM.h"
#include "enc28j60.h"
#include "COM_Generic.h"


extern SPI_HandleTypeDef * hspi1;

ETHERNET_COM::ETHERNET_COM(uint8_t * mac , SPI_TypeDef * SPI_contr , GPIO_TypeDef * GPIO_contr , uint8_t MOSI ,  uint8_t MISO , uint8_t SCK , uint8_t CS , uint8_t AF)
{
	
	ETH_SPI_Handler = new SPI_HandleTypeDef;
	memset(ETH_SPI_Handler , 0 , sizeof(SPI_HandleTypeDef));
	ETH_SPI_Handler->Instance = SPI_contr;
	ETH_SPI_low_level_init(GPIO_contr);
	

	GPIO_InitTypeDef GPIO = {0};
	GPIO.Pin = (1<<MOSI) | (1<<MISO) | (1<<SCK);
	GPIO.Mode = GPIO_MODE_AF_PP;
	GPIO.Pull = GPIO_NOPULL;
	GPIO.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO.Alternate = AF;
	HAL_GPIO_Init (GPIOA, &GPIO);
	GPIO.Pin = (1<<CS);
	GPIO.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO.Pull = GPIO_NOPULL;
	GPIO.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init (GPIO_contr, &GPIO);	
	
  ETH_SPI_Handler->Init.Mode = SPI_MODE_MASTER;
  ETH_SPI_Handler->Init.Direction = SPI_DIRECTION_2LINES;
  ETH_SPI_Handler->Init.DataSize = SPI_DATASIZE_8BIT;
  ETH_SPI_Handler->Init.CLKPolarity = SPI_POLARITY_LOW;
  ETH_SPI_Handler->Init.CLKPhase = SPI_PHASE_1EDGE;
  ETH_SPI_Handler->Init.NSS = SPI_NSS_SOFT;
  ETH_SPI_Handler->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  ETH_SPI_Handler->Init.FirstBit = SPI_FIRSTBIT_MSB;
  ETH_SPI_Handler->Init.TIMode = SPI_TIMODE_DISABLE;
  ETH_SPI_Handler->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  ETH_SPI_Handler->Init.CRCPolynomial = 10;
	HAL_SPI_Init(ETH_SPI_Handler);
	
	HAL_Delay(1);
	hspi1 = ETH_SPI_Handler;
	ENC28_Init(mac);
}


ETHERNET_COM::~ETHERNET_COM()
{
	delete ETH_SPI_Handler;
}

STD_Return_t ETHERNET_COM::Send(char * data , uint32_t len)
{
	ENC28_packetSend((uint8_t *)data, len);
}

STD_Return_t ETHERNET_COM::Receive()
{
	uint16_t len = ENC28J60_packetReceive(receive_buffer ,  COM_BUFFER_MAX_LENGTH - (receive_tracker % COM_BUFFER_MAX_LENGTH));
	if(len > 0)
	{
		receive_tracker += len;
		return STD_OK;
	}
	
	return STD_NOT_OK;
}

SPI_HandleTypeDef * ETHERNET_COM::Get_ETH_SPI_Handle_TypeDef()
{
	return ETH_SPI_Handler;
}

void ETHERNET_COM::Receive_callback()
{
	
}

void ETHERNET_COM::Send_callback()
{

}

void ETHERNET_COM::Interrupt_handler()
{
	HAL_SPI_IRQHandler(ETH_SPI_Handler);
}

void ETHERNET_COM::poll()
{
	if(Receive() == STD_OK)
	{
			Notify_observers(this);
	}
	
	if(!OnGoingTx)
	{
		for(send_handle * item : obsrvables_tracking)
		{
			if(item->data_to_be_sent_tracker < item->data_received_tracker)
			{
				Send((char *)&item->Source_COM->receive_buffer[(item->data_to_be_sent_tracker++)%COM_BUFFER_MAX_LENGTH] , 1);
				return;
			}
		}
	}
}

void ETHERNET_COM::ETH_SPI_low_level_init(GPIO_TypeDef * GPIO_contr)
{
	GPIO_clock_Enable(GPIO_contr);
	switch(((uint32_t )(ETH_SPI_Handler->Instance)))
	{
		case ((uint32_t )(SPI1_BASE)):
			__SPI1_CLK_ENABLE();
			NVIC_EnableIRQ(SPI1_IRQn);
			break;
		case ((uint32_t)(SPI2_BASE)):
			__SPI2_CLK_ENABLE();
			NVIC_EnableIRQ(SPI2_IRQn);
			break;
		case ((uint32_t )(SPI3_BASE)):
			__SPI3_CLK_ENABLE();
			NVIC_EnableIRQ(SPI3_IRQn);
			break;
		case ((uint32_t)(SPI4_BASE)):
			__SPI4_CLK_ENABLE();
			NVIC_EnableIRQ(SPI4_IRQn);
			break;
	}
}
