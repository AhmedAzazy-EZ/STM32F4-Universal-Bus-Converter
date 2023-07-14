/*
 * COM_Generic.h
 *
 * 	Created: 5-July-23
 *  Author: Ahmed Azazy
 */
#ifndef _COM_GENERIC_H_
#define _COM_GENERIC_H_

#ifdef __cplusplus
 extern "C" {
#endif
	 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>	 
#include "stm32f4xx_hal.h"
#include "std_types.h"
#include "Debug.h"
#ifdef __cplusplus
 }
#endif
#include "BASE_COM.h"
#include "UART_COM.h"
	 
	 
#define COM_BUFFER_MAX_LENGTH (500)
	 
#endif