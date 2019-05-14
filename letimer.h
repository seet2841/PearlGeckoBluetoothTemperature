/***************************************************************************//**
 * @file LETIMER.h********************************************************************************
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
#include <stdint.h>
//***********************************************************************************
// defined files
//***********************************************************************************
#define LFXO_FREQ 32768u
#define DESIRED_PERIOD  3 // Period of temperature measurements and transmission
#define LED_ON_TIME .5
#define MAX_FREQ 65535u
#define MAX_EM_ELEMENTS 5
#define ULFO_FREQ 1000u
#define SI7021_INIT_TIME 0.08 // Times needed for SI7021 to initialize after being turned on

#define EM0 0
#define EM1 1
#define EM2 2
#define EM3 3
#define EM4 4

#define TEMP_STRING_LENGTH 7

//***********************************************************************************
// global variables
//***********************************************************************************
uint32_t sleep_block_counter[MAX_EM_ELEMENTS];

//***********************************************************************************
// function prototypes
//***********************************************************************************
void LETIMER_init(int EM1_Mode);
void LETIMER0_IRQHandler();
void Enable_LPM();
void Disable_LPM();
void ftos(float temp, char *str);
int ArrayLength(float number);






