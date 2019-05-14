/***************************************************************************//**
 * @file LETIMER.c********************************************************************************
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

#include "letimer.h"

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "gpio.h"
#include "em_letimer.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_emu.h"
#include "si7021.h"
#include "sleep.h"
#include "main.h"


extern volatile int schedule_event;
extern int CorD;


/**************************************************************************//**
 * @brief Initializes the LETIMER with both COMP0 and COMP1. Sets up the Prescaler equations
 * 		  to calculate the vcomp1 and vcomp2 values needed to achieve a period defined in the DESIRED_PERIOD define statement
 * 		  in the letimer.h file
 *****************************************************************************/
void LETIMER_init(int EM_Mode)
{

/***************************************LETIMER_STRUCT************************************************************/
	LETIMER_Init_TypeDef myTimer;
	myTimer.bufTop = false;
	myTimer.comp0Top = true;
	myTimer.debugRun = false;
	myTimer.enable = false;
	myTimer.out0Pol = 0;
	myTimer.out1Pol = 0;
	myTimer.repMode = letimerRepeatFree;
	myTimer.ufoa0 = letimerUFOANone;
	myTimer.ufoa1 = letimerUFOANone;
	LETIMER_Init(LETIMER0,&myTimer);
/*****************************************************************************************************************/


	uint32_t eff_freq = 0;
	uint32_t vcomp0 = 0;
	eff_freq = LFXO_FREQ;

	if(EM_Mode > 3)
	{
		eff_freq = ULFO_FREQ;
	}

/***************************************PRESCALER_CALCULATIONS************************************************************/
	int pres_cal = 1;

	if(eff_freq * DESIRED_PERIOD > MAX_FREQ)
	{
	while(eff_freq * DESIRED_PERIOD > MAX_FREQ)
		{
		eff_freq = eff_freq >> 1;
		pres_cal = pres_cal << 1;
		}
	}
	else
	{
		pres_cal = 0;
	}


	vcomp0 = (eff_freq-1) * DESIRED_PERIOD;
	uint32_t vcomp1 = vcomp0 - (SI7021_INIT_TIME * eff_freq);


	CMU_ClockPrescSet(cmuClock_LETIMER0, pres_cal);
/*************************************************************************************************************************/


/**************************************************INTERRUPT_SETUP********************************************************/
	LETIMER_CompareSet(LETIMER0,0,vcomp0);
	LETIMER_CompareSet(LETIMER0,1,vcomp1);
	LETIMER0->IFC = LETIMER_IFC_COMP0 | LETIMER_IFC_COMP1;
	LETIMER0->IEN = LETIMER_IEN_COMP0 | LETIMER_IEN_COMP1;

	LETIMER_Enable(LETIMER0,true);

/*************************************************************************************************************************/
}


/**************************************************************************//**
 * @brief  ftos(float temp, char *str) takes in a float that has a temperature value in it
 * 		   and a pointer to an array of characters. The functions converts the float value to
 * 		   a string of characters pertaining to the same float so that it can be sent character by
 * 		   character over LEUART

 * @param  temp: This is a float variable containing the temperature value to be converter to a string (character array)
           *str: This is the pointer to a character array that the function uses the address of
                 to modify the array in real-time without the need for a return statement
 *****************************************************************************/
void ftos(float temp, char *str)
{
	int temp_int = (int)(temp * 10);

	if((temp_int) < 0)
	{
		temp_int = -temp_int;
		if((temp_int % 1000) != temp_int) // Negative temp in the hundreds place
		{
			str[0] = '-';
			str[1] = (char)((temp_int/1000) + 0x30);
			temp_int = (temp_int) % 1000;
			str[2] = (char)((temp_int/100)+0x30);
			temp_int = temp_int % 100;
			str[3] = (char)((temp_int/10) + 0x30);
			str[4] = '.';
			temp_int = temp_int % 10;
			str[5] = (temp_int + 0x30);
			str[6] = 'C';
			if(CorD == 1)
			{
				str[6] = 'F';
			}
			return;
		}

		if((temp_int % 100) < temp_int) // Negative temp in the tens place
		{
			str[0] = '-';
			str[1] = (char)(' ');
			str[2] = (char)(((temp_int)/100) + 0x30);
			temp_int = (temp_int % 100);
			str[3] = (char)(((temp_int)/10) + 0x30);
			temp_int = temp_int%10;
			str[4] = '.';
			str[5] = (char)(temp_int+0x30);
			str[6] = 'C';
			if(CorD == 1)
			{
				str[6] = 'F';
			}
			return;
		}

		if((temp_int % 10) > 0) // Negative temp in the ones place
		{
			str[0] = '-';
			str[1] = (char)(0x20);
			str[2] = (char)(0x20);
			str[3] = (char)((temp_int/10) + 0x30);
			str[4] = (char)('.');
			str[5] = (char)((temp_int % 10) + 0x30);
			str[6] = 'C';
			if(CorD == 1)
			{
				str[6] = 'F';
			}
			return;
		}
	}

	if((temp_int % 1000) != temp_int) // Positive temp in the hundreds place
	{
	str[0] = '+';
	str[1] = (char)((temp_int/1000) + 0x30);
	if(str[1] == 0)
	{
		str[1] = ' ';
	}
	temp_int = (temp_int) % 1000;
	str[2] = (char)((temp_int/100)+0x30);
	temp_int = temp_int % 100;
	str[3] = (char)((temp_int/10) + 0x30);
	str[4] = '.';
	temp_int = temp_int % 10;
	str[5] = (temp_int + 0x30);
	str[6] = 'C';
	if(CorD == 1)
	{
		str[6] = 'F';
	}
	return;
	}

	if((temp_int % 100) < temp_int) // Positive temp in the tens place
	{
	str[0] = '+';
	str[1] = ' ';
	str[2] = (char)(((temp_int)/100) + 0x30);
	temp_int = (temp_int % 100);
	str[3] = (char)(((temp_int)/10) + 0x30);
	temp_int = temp_int%10;
	str[4] = '.';
	str[5] = (char)(temp_int+0x30);
	str[6] = 'C';
	if(CorD == 1)
	{
		str[6] = 'F';
	}
	return;
	}

	if((temp_int % 10) > 0) //Positive temp in the ones place
	{
		str[0] = '+';
		str[1] = ' ';
		str[2] = ' ';
		str[3] = (char)((temp_int/10) + 0x30);
		str[4] = (char)('.');
		str[5] = (char)((temp_int % 10) + 0x30);
		str[6] = 'C';
		if(CorD == 1)
		{
			str[6] = 'F';
		}
		return;
	}



}


/**************************************************************************//**
 * @brief
 *   LETIMER interrupt handler.
 *
 * @details
 *   LETIMER_IRQHandler() is an ISR that sets the scheduler flags for COMP0 and COMP1 events to be serviced
 *   in the main program
 *****************************************************************************/
void LETIMER0_IRQHandler() {
	uint32_t flag = (LETIMER0->IF & LETIMER0->IEN);
	LETIMER0->IFC = flag;
	 CORE_ATOMIC_IRQ_DISABLE(); // Disabled Processor interrupts at beginning of handler
	if( flag & LETIMER_IF_COMP0)
	{
		LETIMER0->IEN &= ~(LETIMER_IEN_COMP0); // Disable COMP0 interrupts while being serviced
		schedule_event |= COMP0_Interrupt; // Set COMP0 scheduler flag
	}
	if( flag & LETIMER_IF_COMP1)
	{
		LETIMER0->IEN &= ~(LETIMER_IEN_COMP1); // Disable COMP1 interrupts while being serviced
		schedule_event |= COMP1_Interrupt; // Set COMP1 scheduler flag
	}
	CORE_ATOMIC_IRQ_ENABLE(); // Enable processor interrupts at end of Handler

}
/***************************************************************************************************************************/



