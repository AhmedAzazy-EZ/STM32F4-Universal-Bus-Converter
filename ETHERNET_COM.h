/*
 * ETHERNET_COM.h
 *
 * 	Created: 7-August-23
 *  Author: Ahmed Azazy
 */
 
#ifndef _ETHERNET_COM_H_
#define _ETHERNET_COM_H_

#include "BASE_COM.h"



class ETHERNET_COM : public BASE_COM_t
{
public:
		ETHERNET_COM(uint8_t * mac , SPI_TypeDef * SPI_contr , GPIO_TypeDef * GPIO_contr , uint8_t MOSI ,  uint8_t MISO , uint8_t SCK , uint8_t CS , uint8_t AF);
		~ETHERNET_COM();
		STD_Return_t Send(char * data , uint32_t len) override;
		STD_Return_t Receive()override;
		SPI_HandleTypeDef * Get_ETH_SPI_Handle_TypeDef();
		void Receive_callback()override;
		void Send_callback() override;
		void Interrupt_handler()override;
		void poll() override;

private:
	SPI_HandleTypeDef * ETH_SPI_Handler;
	void ETH_SPI_low_level_init(GPIO_TypeDef * GPIO_contr);
};



/*


HAL_StatusTypeDef GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIO = {0};
	
	
	GPIO.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO.Mode = GPIO_MODE_AF_PP;
	GPIO.Pull = GPIO_NOPULL;
	GPIO.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init (GPIOA, &GPIO);
	
	
	GPIO.Pin = GPIO_PIN_4;
	GPIO.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO.Pull = GPIO_NOPULL;
	GPIO.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init (GPIOA, &GPIO);

	return HAL_OK;
}

*/

#endif