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

#include "cmu.h"
#include "letimer.h"
#include "sleep.h"
#include "em_core.h"
#include "em_emu.h"

/**************************************************************************//**
 * @brief Sets minimum energy mode processor can sleep in
 *****************************************************************************/
void blockSleepMode(int minimumMode)
{
	CORE_ATOMIC_IRQ_DISABLE();
	sleep_block_counter[minimumMode]++;
	CORE_ATOMIC_IRQ_ENABLE();
}

/**************************************************************************//**
 * @brief unblocks minimum sleep mode that processor can sleep in
 *****************************************************************************/
void unblockSleepMode(int minimumMode)
{
	CORE_ATOMIC_IRQ_DISABLE();
	if(sleep_block_counter[minimumMode] > 0)
	{
		sleep_block_counter[minimumMode]--;
	}
	CORE_ATOMIC_IRQ_ENABLE();
}

/**************************************************************************//**
 * @brief Enters processor into energy mode (lowest is whatever sleep mode is blocked)
 *****************************************************************************/
void sleep(void)
{
	if(sleep_block_counter[EM0] > 0)
	{
		return; // Stay in EM0
	}
	else if(sleep_block_counter[EM1] > 0)
	{
		return; // EM1 is blocked, so o into EM0
	}
	else if(sleep_block_counter[EM2] > 0)
	{
		EMU_EnterEM1(); // EM2 is blocked, so go into EM1
	}
	else if(sleep_block_counter[EM3] > 0)
	{
		EMU_EnterEM2(true); // EM3 is blocked, so go into EM2
	}
	else
	{
		EMU_EnterEM3(true); // Don't go into EM4
	}
	return;
}
