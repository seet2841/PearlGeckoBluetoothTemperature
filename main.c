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

/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for SLSTK3402A
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_core.h"
#include "bsp.h"
#include "main.h"
#include "gpio.h"
#include "cmu.h"
#include "em_letimer.h"
#include "i2c.h"
#include "letimer.h"
#include "em_i2c.h"
#include "sleep.h"
#include "si7021.h"
#include "leuart.h"
#include "em_leuart.h"
#include "em_ldma.h"
#include "ldma.h"
#include "cryotimer.h"
#include "capsense.h"
#include "capsenseconfig.h"
#include "em_cryotimer.h"



volatile int schedule_event = 0; // Initialize state variable for scheduler system
//char string[12] = "AT+NAMEseet"; // Transmit string
int AsciiIndex = 0; // Index to keep track of transmit string
char array[TEMP_STRING_LENGTH];
extern char data[RECEIVESIZE];
int STARTFBUFFER=0;
int CorD = 0; // Flag for Celsius or Farenheit
extern LDMA_TransferCfg_t RXConfig; // RX Config struct
extern LDMA_Descriptor_t RXDescript; // RX Descriptor
int ToggleTemp = 1;






int main(void)
{

  CORE_ATOMIC_IRQ_DISABLE();
  blockSleepMode(EM3);
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFXO and disable HFRCO */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

  /***************************************PERIPHERAL_INIT************************************************************/
  cmu_init(EM3); // Enable Clocks
  for(int k = 0; k < 1000000; k++);

  gpio_init(); // Set up GPIO Pins
  for(int k = 0; k < 1000000; k++);

  LETIMER_init(EM3); //  Set up LETIMER Peripheral

 // for(int k = 0; k < 1000000; k++);

  i2c_init(); // Set up I2C lines;
  //for(int k = 0; k < 1000000; k++);


  leuart_init(); // Set up LEUART Peripheral

  LDMA_SETUP(); // Setup LDMA

  LDMA_Receive(data, RECEIVESIZE); // 1st instance of LDMA receive to jumpstart the rest

  Cryo_Setup(); // CRYOTIMER setup

  CAPSENSE_Init(); // Initialize capacitive sensing mechanism


  /******************************************************************************************************************/

  NVIC_EnableIRQ(LEUART0_IRQn); // Enable peripheral level interrupts
  NVIC_EnableIRQ(LETIMER0_IRQn); // Enable peripheral level interrupts
  CORE_ATOMIC_IRQ_ENABLE(); // Enable processor interrupts


  while (1) {
	  // If no interrupts are pending, go to sleep
	  if(schedule_event == 0)
	  {
		  sleep();
	  }


	  if(schedule_event & SIGF_Interrupt)
	  {
		  STARTFBUFFER = 0;
		  //Decode
		  for(int i = 0; i < RECEIVESIZE-1 ; i++)
		  {
		  if(((data[i] == ('d')) || (data[i] == 'D')) && ((data[i+1] == ('f')) || (data[i+1] == 'F')))
		  {
			  CorD = 1;
		  }
		  if(((data[i] == ('d')) || (data[i] == 'D')) && ((data[i+1] == ('c')) || (data[i+1] == 'C')))
		  {
			  CorD = 0;

		  }
		  }


		  LDMA_StartTransfer(RX_DMA_CHANNEL, &RXConfig, &RXDescript); // Restarts the DMA transfer with reset index
		  schedule_event &= ~(SIGF_Interrupt);
	  }
	  if(schedule_event & COMP0_Interrupt)
	  {
			GPIO_PinOutSet(TEMP_SENSOR_port,TEMP_SENSOR_pin); // Turn on Temp Sensor
			LETIMER0->IEN |= (LETIMER_IEN_COMP0); // Reenable COMP0 Interrupts (Disabled in Handler)
			schedule_event &= ~(COMP0_Interrupt); // Clear scheduler flag
	  }
	  if(schedule_event & COMP1_Interrupt)
	  {
		  Enable_LPM(); // Enable Load Power Management
		  uint16_t TEMP_CODE = temp_read(); // Read 16 bit temp code

	      float degree = convert_tempCel(TEMP_CODE); // Convert temp code to celcius
		  if(CorD == 1)
		  {
			   degree = (degree * 9/5) + 32; // Convert temp to farenheit
		  }
		  ftos(degree, array); // Convert celcius temp to string and store in array
		  LDMA_Transfer(array, TEMP_STRING_LENGTH); // Prompts the DMA to start transferring the passed in array over LEUART
		  Disable_LPM(); // Disable Load Power Management
		  schedule_event &= ~(COMP1_Interrupt); // Clear Scheduler flag
		  LETIMER0->IEN |= (LETIMER_IEN_COMP1); // Enable COMP1 interrupts
	  }
	  if(schedule_event & CRYO_EVENT)
	  {
		  CRYOTIMER->IEN &= ~(CRYOTIMER_IEN_PERIOD); // Disables CYROTIMER interrupt during servicing
		  CAPSENSE_Sense(); // Senses the status of each button channel
		  if (CAPSENSE_getPressed(BUTTON0_CHANNEL))
		  {
			  ToggleTemp ^= 1; // Toggles a bit that indicates state of temperature reading
			  if(ToggleTemp == 0)
			  {
				  TempDisable(); // Disable Temperature reading/transmitting

			  }
			  if(ToggleTemp == 1)
			  {
				  TempEnable(); // Enabling Temperature reading/transmitting
			  }
		  }
		  CRYOTIMER->IEN |= CRYOTIMER_IEN_PERIOD; // Enable CRYOTIMER interrupt after servicing
		  schedule_event &= ~(CRYO_EVENT); // Unschedule Event
	  }



  }

}
