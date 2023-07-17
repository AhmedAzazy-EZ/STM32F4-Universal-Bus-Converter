/*
 * I2C_COM.cpp
 *
 * 	Created: 17-July-23
 *  Author: Ahmed Azazy
 */
 
 #include "I2C_COM.h"
 
I2C_COM::I2C_COM(uint32_t ClockSpeed , uint32_t Own_Add , I2C_TypeDef * I2c_Contr , GPIO_TypeDef * GPIO_contr , uint32_t SCK_PinNum , uint32_t SDA_PinNum , uint32_t AF)
{
	I2C_Handler = new I2C_HandleTypeDef;
	memset(I2C_Handler, 0, sizeof(I2C_HandleTypeDef));
	I2C_Handler->Instance = I2c_Contr;
	I2C_low_level_init(GPIO_contr);

}



void I2C_COM::I2C_low_level_init(GPIO_TypeDef * GPIO_contr)
{
	GPIO_clock_Enable(GPIO_contr);
	
	switch((uint32_t )I2C_Handler->Instance)
	{
		case (uint32_t) I2C1_BASE : 
			__I2C1_CLK_ENABLE();
			NVIC_EnableIRQ(I2C1_EV_IRQn);
			NVIC_EnableIRQ(I2C1_ER_IRQn);
			break;
		case (uint32_t) I2C2_BASE : 
			__I2C2_CLK_ENABLE();
			NVIC_EnableIRQ(I2C2_EV_IRQn);
			NVIC_EnableIRQ(I2C2_ER_IRQn);
			break;
		case (uint32_t) I2C3_BASE : 
			__I2C3_CLK_ENABLE();
			NVIC_EnableIRQ(I2C3_EV_IRQn);
			NVIC_EnableIRQ(I2C3_ER_IRQn);
			break;
	}

}