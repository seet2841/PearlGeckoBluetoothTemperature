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
#include "cmu.h"

#include "letimer.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

/**************************************************************************//**
 * @brief Setup clocks for all peripherals on the Pearl Gecko
 * @param EM_Mode The Energy mode that is being blocked in the rest of the program.
 *****************************************************************************/


void cmu_init(int EM_Mode){


//****************************HIGH FREQUENCY CLOCKS FOR I2C************************************************//
		CMU_ClockSelectSet(cmuClock_HFPER, cmuSelect_HFXO);
		CMU_ClockEnable(cmuClock_HFPER, true);
//********************************************************************************************************//

		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	// using LFXO or ULFRCO


//****************************IF EM4; USE ULTRA LOW CLOCKS; ELSE USE LOW CLOCKS***************************//
		if(EM_Mode > 3)
		{
		CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
		CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
		}
		else
		{
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);		// Disable LFXO
		CMU_OscillatorEnable(cmuOsc_HFRCO,true,true);       // Oscillator for LDMA
		CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);    // Oscillator for CYROTIMER
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);	// routing clock to LFA
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);	// routing clock to LFA
		CMU_ClockSelectSet(cmuClock_HFPER, cmuSelect_HFRCO); // Select Clock for LDMA
		CMU_ClockSelectSet(cmuClock_CRYOTIMER, cmuSelect_ULFRCO); // Select Clock for CRYOTIMER
		}
//*******************************************************************************************************//
		CMU_ClockEnable(cmuClock_LFA, true);    // Enable LETIMER clock
		CMU_ClockEnable(cmuClock_LFB, true);   // Enable LEUART clock
		CMU_ClockEnable(cmuClock_HFPER,true);  // Enable LDMA clock
		CMU_ClockEnable(cmuClock_CORELE, true);

//****************************ENABLE PERIPHERAL CLOCKS************************************************//
		CMU_ClockEnable(cmuClock_GPIO, true); // Enables GPIO Clock
		CMU_ClockEnable(cmuClock_LETIMER0, true); // Enables LETIMER Clock
		CMU_ClockEnable(cmuClock_I2C0, true); // Enables I2C clock
		CMU_ClockEnable(cmuClock_LEUART0, true); // Enables LEUART clock
		CMU_ClockEnable(cmuClock_LDMA,true); // Enables LDMA clock
		CMU_ClockEnable(cmuClock_CRYOTIMER,true); // Enables Cyrotimer Clock
//****************************************************************************************************//
		CMU_HFXOAutostartEnable(true, false, false); // For Expected Energy Results




}


