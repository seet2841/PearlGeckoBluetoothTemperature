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



#include "leuart.h"
#include "em_leuart.h"
#include "letimer.h"
#include "sleep.h"
#include "em_core.h"
#include "main.h"
#include "ldma.h"


extern volatile int schedule_event; // Global variable that acts as interrupt state variable
char data[RECEIVESIZE];
extern int STARTFBUFFER;


/**************************************************************************//**
 * @brief leuart_init() is a function designed to initialize the peripheral at 9600 baud, route the tX and RX pins
 * Configure the START and STOP frames, and enable SIGF interrupts as well as enabling the peripheral
 *****************************************************************************/
void leuart_init()
{
	LEUART_Init_TypeDef myLEUART;
    myLEUART.enable = leuartEnable;    /* Enable RX/TX when initialization completed. */                      \
    myLEUART.refFreq = Ref;               /* Use current configured reference clock for configuring baud rate.*/ \
    myLEUART.baudrate = Baud;            /* 9600 bits/s. */                                                     \
    myLEUART.databits = leuartDatabits8; /* 8 data bits. */                                                     \
    myLEUART.parity = leuartNoParity;  /* No parity. */                                                       \
    myLEUART.stopbits = leuartStopbits1;  /* 1 stop bit. */

	LEUART_Init(LEUART0, &myLEUART); // Initialize Struct

	for(int i = 0; i < 1000000; i++);

    LEUART0->ROUTELOC0 = (LEUART_ROUTELOC0_RXLOC_LOC18 | LEUART_ROUTELOC0_TXLOC_LOC18); // Routing Locations
    LEUART0->ROUTEPEN |= (LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN); // Enable TX and RX on route provided

    LEUART0->IFC = LEUART_IF_RXDATAV | LEUART_IF_TXBL; // Clears these interrupt flags
    LEUART0->IEN |= LEUART_IEN_SIGF; // Enable these register flags
    LEUART0->CTRL |= LEUART_CTRL_SFUBRX|  LEUART_CTRL_TXDMAWU; //
    LEUART0->STARTFRAME = '?';
    LEUART0->SIGFRAME = '#';

    LEUART_Enable(LEUART0,leuartEnable); // Enable the LEUART0 peripheral


}

/**************************************************************************//**
 * @brief
 *   LEUART0 interrupt handler.
 *
 * @details
 *    The LEUART0_IRQHandler(void) function is an ISR that provides different functionality based on which interrupt
      triggers the ISR (for example STARTF, RXDATAV, SIGF, ect)
 *****************************************************************************/
void LEUART0_IRQHandler(void)
{
	CORE_ATOMIC_IRQ_DISABLE(); // Disable Processor interrupts at beginning of Handler
	uint32_t flag2 = (LEUART0->IF & LEUART0->IEN);
	LEUART0->IFC = flag2;

	if( flag2 & LEUART_IF_STARTF)
	{
		LEUART0->CMD = LEUART_CMD_RXBLOCKDIS; // Disables RX block on STARTF interrupt (obselete after LDMA)
	}
	if( flag2 & LEUART_IF_RXDATAV)
	{
		LEUART0->IEN &= ~LEUART_IEN_RXDATAV;
		data[STARTFBUFFER] = LEUART0->RXDATA; // Buts character in buffer into data array (obselete after LDMA)
		STARTFBUFFER++;
	}

	if( flag2 & LEUART_IF_SIGF)
	{
		LDMA_StopTransfer(RX_DMA_CHANNEL); // Stops transfer to reset the transfer index
		//LEUART0->IEN &= ~LEUART_IEN_RXDATAV;
		LEUART0->CMD = LEUART_CMD_RXBLOCKEN; // Reenable the RX block
		unblockSleepMode(EM3); // Unblocks sleep mode
		schedule_event |= SIGF_Interrupt; // Setting SIGF flag for scheduler
	}

	if((flag2 & LEUART_IF_TXBL) || (flag2 & LEUART_IF_TXC))
	{
		LEUART0->IEN &= ~(LEUART_IEN_TXBL | LEUART_IEN_TXC); // Disable TXC and TXBL interrupts while being serviced by scheduler
		unblockSleepMode(EM3); // unblocks EM3
	}


	 CORE_ATOMIC_IRQ_ENABLE(); //Enable Processor interrupts at the beginning of Handler

}


