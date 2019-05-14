/***************************************************************************//**
 * @file si7021.h********************************************************************************
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
//***********************************************************************************
// defined files
//***********************************************************************************
#define SLAVE_ADDRESS 0x40
#define WRITE_BIT 0
#define READ_BIT 1
#define READ_ADDRESS 0xE7
#define WRITE_ADDRESS 0xE6
#define TEMP_SENSOR_RES    0x01
#define MEASURE_CMD 0xE3
#define DEFINED_TEMP 15

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void write_cmd(uint8_t command);
uint8_t read_cmd();
uint16_t temp_read();
float convert_tempCel(uint16_t data);
