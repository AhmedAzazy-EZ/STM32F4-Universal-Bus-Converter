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
		ETHERNET_COM(char * mac , SPI_TypeDef * SPI_contr , GPIO_TypeDef * GPIO_contr , uint8_t MOSI ,  uint8_t MISO , uint8_t SCK , uint8_t CS);
		~ETHERNET_COM();
		SPI_HandleTypeDef * Get_SPI_Handle_TypeDef();
};



#endif