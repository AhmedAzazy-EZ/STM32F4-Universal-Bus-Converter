/*
 * I2C_COM.cpp
 *
 * 	Created: 17-July-23
 *  Author: Ahmed Azazy
 */
 
 #include "COM_Generic.h"
 #include "I2C_COM.h"
 
I2C_COM::I2C_COM(uint32_t ClockSpeed , uint32_t Own_Add , uint16_t My_Slave_Add , I2C_TypeDef * I2c_Contr , GPIO_TypeDef * GPIO_contr , uint32_t SCK_PinNum , uint32_t SDA_PinNum , uint32_t AF)
{
	I2C_Handler = new I2C_HandleTypeDef;
	memset(I2C_Handler, 0, sizeof(I2C_HandleTypeDef));
	I2C_Handler->Instance = I2c_Contr;
	I2C_low_level_init(GPIO_contr);
	Slave_Add = My_Slave_Add;
	GPIO_InitTypeDef _GPIO = {0};
	_GPIO.Pin = (1 << SCK_PinNum) | (1 << SDA_PinNum);
	_GPIO.Mode = GPIO_MODE_AF_OD;
	_GPIO.Pull = GPIO_PULLUP;
	_GPIO.Alternate = AF;
	_GPIO.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIO_contr , &_GPIO);
	
	I2C_Handler->Instance = I2c_Contr;
	I2C_Handler->Init.ClockSpeed = 100000;
	I2C_Handler->Init.DutyCycle = I2C_DUTYCYCLE_2;
	I2C_Handler->Init.OwnAddress1 = Own_Add;
	I2C_Handler->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2C_Handler->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2C_Handler->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2C_Handler->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;	
	HAL_I2C_Init(I2C_Handler);
	
	Receive();
	
}


I2C_COM::~I2C_COM()
{
	delete I2C_Handler;
	delete this;
}

void I2C_COM::I2C_low_level_init(GPIO_TypeDef * GPIO_contr)
{
	GPIO_clock_Enable(GPIO_contr);
	
	switch((uint32_t )I2C_Handler->Instance)
	{
		case (uint32_t) I2C1_BASE : 
			__I2C1_CLK_ENABLE();
			NVIC_EnableIRQ(I2C1_EV_IRQn);
			//NVIC_EnableIRQ(I2C1_ER_IRQn);
			break;
		case (uint32_t) I2C2_BASE : 
			__I2C2_CLK_ENABLE();
			NVIC_EnableIRQ(I2C2_EV_IRQn);
			//NVIC_EnableIRQ(I2C2_ER_IRQn);
			break;
		case (uint32_t) I2C3_BASE : 
			__I2C3_CLK_ENABLE();
			NVIC_EnableIRQ(I2C3_EV_IRQn);
			//NVIC_EnableIRQ(I2C3_ER_IRQn);
			break;
	}

}

I2C_HandleTypeDef * I2C_COM::Get_I2c_Handle_TypeDef()
{
	return I2C_Handler;
}

STD_Return_t I2C_COM::Send(char * data , uint32_t len)
{
	HAL_I2C_Master_Transmit_IT(I2C_Handler , Slave_Add , (uint8_t *)data , 1);
	OnGoingTx = true;
	return true;
}

STD_Return_t I2C_COM::Receive()
{
	HAL_I2C_Slave_Receive_IT(I2C_Handler , (uint8_t *)&receive_buffer[++receive_tracker%COM_BUFFER_MAX_LENGTH] , 1 );
	return true;
}

void I2C_COM::Receive_callback()
{
	Receive();
	Notify_observers(this);
}

void I2C_COM::Send_callback()
{
	OnGoingTx = false;
}

void I2C_COM::Interrupt_handler()
{
	HAL_I2C_EV_IRQHandler(I2C_Handler);
}