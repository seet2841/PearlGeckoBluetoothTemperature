

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

#include "em_cryotimer.h"
#include "em_cmu.h"
#include "em_core.h"
#include "gpio.h"
#include "em_gpio.h"
#include "cryotimer.h"
#include "em_letimer.h"
#include "em_leuart.h"


extern volatile int schedule_event;

/**************************************************************************//**
 * @brief Sets up the Cryotimer for a period of 1 second
 *****************************************************************************/

void Cryo_Setup()
{
		CRYOTIMER_Init_TypeDef cryostruct;
		cryostruct.enable = false; // Doesn't enable timer right away
		cryostruct.debugRun = false; // Debugrun disabled
		cryostruct.em4Wakeup = true; // Allows timer to wakeup from EM4
		cryostruct.osc = cryotimerOscULFRCO; // Sets up timer with ULFRCO
		cryostruct.presc = cryotimerPresc_1; // No prescaling
		cryostruct.period = cryotimerPeriod_1k; // periodselect = 10
		CRYOTIMER_Init(&cryostruct); // Initializes LDMA peripheral

		CRYOTIMER->IFC = CRYOTIMER_IFC_PERIOD; // Clears previous interrupts
		CRYOTIMER->IEN |= CRYOTIMER_IEN_PERIOD; // Enables Period Interrupt on CRYOTIMER
		NVIC_EnableIRQ(CRYOTIMER_IRQn); // Enables Interrupts for the LDMA

		CRYOTIMER_Enable(true); // Enable CRYOTIMER Peripheral
}

/**************************************************************************//**
 * @brief Disables the writing of Temperature measurements over bluetooth
 *****************************************************************************/
void TempDisable()
{
	LETIMER0->CMD = (LETIMER_CMD_STOP | LETIMER_CMD_CLEAR); // Stops LETIMER and clears current CNT value
	//NVIC_DisableIRQ(LETIMER0_IRQn);
	GPIO_PinOutClear(TEMP_SENSOR_port,TEMP_SENSOR_pin); // Turn off Temp Sensor

}

/**************************************************************************//**
 * @brief Enables the writing of Temperature measurements over bluetooth
 *****************************************************************************/
void TempEnable()
{
  LETIMER0->CNT = 0; // Set Timer counter to 0 (starts from beginning)
  LETIMER0->CMD = LETIMER_CMD_START; // Restarts the LETIMER
  GPIO_PinOutSet(TEMP_SENSOR_port,TEMP_SENSOR_pin); // Turn on Temp Sensor
}

/**************************************************************************//**
 * @brief
 *   CRYOTIMER interrupt handler.
 *
 * @details
 *   When CRYOTIMER expires it sets the scheduler flag for the capacitive sensing code to be serviced in main
 *****************************************************************************/
void CRYOTIMER_IRQHandler()
{
	CORE_ATOMIC_IRQ_DISABLE(); // Disable Processor interrupts at beginning of Handler
	int flag = CRYOTIMER->IF; // Get flag
	CRYOTIMER->IFC = flag; // Clear flag

	if( flag & CRYOTIMER_IF_PERIOD)
	{
		schedule_event |= CRYO_EVENT; // Set Scheduler flag for Cryotimer

	}
	CORE_ATOMIC_IRQ_ENABLE(); // Enable Processor interrupts at beginning of handler
}



