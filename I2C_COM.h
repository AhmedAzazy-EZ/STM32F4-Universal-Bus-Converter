/*
 * I2C_COM.h
 *
 * 	Created: 17-July-23
 *  Author: Ahmed Azazy
 */
 
 #ifndef _I2C_COM_H_
 #define _I2C_COM_H_
 
 #include "BASE_COM.h"
 
 
 class I2C_COM : public BASE_COM_t
 {
 
 public:
	I2C_COM(uint32_t ClockSpeed , uint32_t Own_Add , I2C_TypeDef * I2c_Contr , GPIO_TypeDef * GPIO_contr , uint32_t SCK_PinNum , uint32_t SDA_PinNum , uint32_t AF);
	~I2C_COM();
	I2C_HandleTypeDef * Get_I2c_Handle_TypeDef();
	STD_Return_t Send(char * data , uint32_t len) override;
	STD_Return_t Receive()override;
	void Receive_callback()override;
  void Send_callback() override;
	void Interrupt_handler()override;

private:
	void I2C_low_level_init(GPIO_TypeDef * GPIO_contr);
	I2C_HandleTypeDef * I2C_Handler;	
 }; 
 
 #endif