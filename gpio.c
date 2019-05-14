/***************************************************************************//**
 * @file sleep.c********************************************************************************
 * @section License* <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 * *********************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,*
 * including commercial applications, and to alter it and redistribute it*
 * freely, subject to the following restrictions:**
 *
 * 1. The origin of this software must not be misrepresented; you must not*
 * claim that you wrote the original software.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be*
 * misrepresented as being the original software.*
 * 3. This notice may not be removed or altered from any source distribution.**
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no*
 * obligation to support this Software. Silicon Labs is providing the*
 * Software "AS IS", with no express or implied warranties of any kind,*
 * including, but not limited to, any implied warranties of merchantability*
 * or fitness for any particular purpose or warranties against infringement*
 * of any proprietary rights of a third party.**
 *
 * Silicon Labs will not be liable for any consequential, incidental, or*
 * special damages, or any other relief, or for any claim by any third party,*
 * arising from your use of this Software.
 ********************************************************************************/

//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************

/**************************************************************************//**
 * @brief Dictate the drive strengths for several on-board peripheral pins and the mode that they're configured for
 *****************************************************************************/

void gpio_init(void){

	/******************************Configuring LED Ports****************************************************************/
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

    //GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);
	/********************************************************************************************************************/



	/******************************I2C Config SDA/SCL and Temp Sensor****************************************************************/
	GPIO_PinModeSet(SCL_port,SCL_pin,gpioModeDisabled,SDA_default);
	GPIO_PinModeSet(SDA_port,SDA_pin,gpioModeDisabled,SCL_default);

	GPIO_PinModeSet(TEMP_SENSOR_port,TEMP_SENSOR_pin,gpioModeDisabled,TEMP_SENSOR_default);
	GPIO_PinOutClear(TEMP_SENSOR_port,TEMP_SENSOR_pin);
	/********************************************************************************************************************************/


	/******************************LEUART Config RX/TX****************************************************************/
	GPIO_DriveStrengthSet(LEUART_TX_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LEUART_TX_PORT, LEUART_TX_PIN, gpioModePushPull, LEUART_TX_default);

	GPIO_DriveStrengthSet(LEUART_RX_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LEUART_RX_PORT, LEUART_RX_PIN, gpioModeInput, LEUART_RX_default);
	/********************************************************************************************************************************/


}
