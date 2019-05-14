/***************************************************************************//**
 * @file gpio.h********************************************************************************
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
#include "main.h"
#include "em_gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************

// LED0 pin is
#define	LED0_port    gpioPortF
#define LED0_pin   4
#define LED0_default	false 	// off

// LED1 pin is
#define LED1_port    gpioPortF
#define LED1_pin   5
#define LED1_default	false	// off

// SCL Pin is
#define SCL_port gpioPortC
#define SCL_pin 11

// SDA Pin is
#define SDA_port gpioPortC
#define SDA_pin 10

#define SCL_default true
#define SDA_default true

//TEMP_SENSOR Pin is
#define TEMP_SENSOR_port gpioPortB
#define TEMP_SENSOR_pin 10
#define TEMP_SENSOR_default true



// LEUART TX Pin is

#define LEUART_TX_PORT     gpioPortD
#define LEUART_TX_PIN       10
#define LEUART_TX_default   true

// LEUART RX Pin is

#define LEUART_RX_PORT     gpioPortD
#define LEUART_RX_PIN       11
#define LEUART_RX_default   true

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_init(void);

