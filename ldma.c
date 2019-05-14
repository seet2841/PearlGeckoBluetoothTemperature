/*
 * ldma.c
 *
 *  Created on: Apr 2, 2019
 *      Author: Selasi Etchey
 */


/***************************************************************************//**
 * @file sleep.h********************************************************************************
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

#include "em_ldma.h"
#include "letimer.h"
#include "ldma.h"
#include "em_core.h"
#include "em_leuart.h"
#include "leuart.h"
#include "letimer.h"
#include "sleep.h"


LDMA_TransferCfg_t TXConfig; // LDMA Transmission Configuration struct
LDMA_Descriptor_t TXDescript; // LDMA Transmission Descriptor struct

LDMA_TransferCfg_t RXConfig; // LDMA Reception Configuration struct
LDMA_Descriptor_t RXDescript; // LDMA Reception Descriptor struct

/**************************************************************************//**
 * @brief LDMA_SETUP() is a function designed to initialize the peripheral and enable peripheral interrupts for the LDMA
 *****************************************************************************/
void LDMA_SETUP()
{
	LDMA_Init_t ldmaInit;
	ldmaInit.ldmaInitCtrlNumFixed = 0;
	ldmaInit.ldmaInitCtrlSyncPrsClrEn = 0;
	ldmaInit.ldmaInitCtrlSyncPrsSetEn = 0;
	ldmaInit.ldmaInitIrqPriority = 3;

	NVIC_EnableIRQ(LDMA_IRQn); // Enables Interrupts for the LDMA
	LDMA_Init(&ldmaInit); // Initializes LDMA peripheral

}

/**************************************************************************//**
 * @brief  LDMA_Transfer(char * outputData, int TransferSize) is a function designed to take in two parameters and transfer data from the memory being passed in
           To the LEUART->TX buffer to be transmitted over bluetooth. In this function the configuration and descriptor for the LDMA are both configured to send the data over LEUART
           and physically send the data

 * @param  outputData: This is a string of information that is going to be sent over LEUART.
           TransferSize: This is the size of the string that is being sent over LEUART
 *****************************************************************************/
void LDMA_Transfer(char * outputData, int TransferSize)
{
	blockSleepMode(EM3); // Blocks EM3
	  LEUART0->CTRL |= LEUART_CTRL_TXDMAWU; // Enables TX DMA Wakeup controller
	  LEUART0->IEN &= ~LEUART_IEN_TXBL; // Disables TXBL
		while(LEUART0->SYNCBUSY); // Needed because CTRL register is in low frequency domain

	  TXDescript = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(outputData, &(LEUART0->TXDATA), TransferSize); // Sets up descriptor with array as source, LEUART0->TXDATA as destination, and the transfer size
	  TXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_LEUART0_TXBL); // Sets up LEUART TXBL as trigger

	  //LDMA->IFC = TX_DMA_CHANNEL;
	  LDMA->IEN |= (1<< TX_DMA_CHANNEL); // Enables interrupts on TX channel

	  while(LEUART0->SYNCBUSY); // Needed because CTRL register is in low frequency domain

	  LDMA_StartTransfer(TX_DMA_CHANNEL, &TXConfig, &TXDescript); // Starts data transfer

}


/**************************************************************************//**
 * @brief     LDMA_Receive(char * inputArray, int ReceiveSize) is a function that takes in an empty array and uses the LEUART0->RXDATA
              Register to fill it with characters transmitted from the phone to the Pearl Gecko over bluetooth

 * @param     inputArray: An empty array that is passed in to the function to be filled used DMA
              ReceiveSize: The size of the empty array that is passed in
 *****************************************************************************/
void LDMA_Receive(char * inputArray, int ReceiveSize)
{
	blockSleepMode(EM3); // Blocks EM3
	  LEUART0->CTRL |= LEUART_CTRL_RXDMAWU; // Enables RX DMA Wakeup controller
	  LEUART0->IEN &= ~LEUART_IEN_RXDATAV; // Disables RXDATAV Interrupts
		while(LEUART0->SYNCBUSY); // Needed because CTRL register is in low frequency domain

	  RXDescript = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(LEUART0->RXDATA), inputArray, ReceiveSize); // Sets up descriptor with LEUART0->RXDATA as source, inputArray as destination, and the receive size
	  RXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_LEUART0_RXDATAV); // Sets up LEUART RXDATAV as trigger

	  //LDMA->IFC = TX_DMA_CHANNEL;
	  LDMA->IEN |= (1<< RX_DMA_CHANNEL); // Enables interrupts on RX channel

	  while(LEUART0->SYNCBUSY); // Needed because CTRL register is in low frequency domain

	  LDMA_StartTransfer(RX_DMA_CHANNEL, &RXConfig, &RXDescript); // Starts data transfer

}


/**************************************************************************//**
 * @brief
 *   LDMA interrupt handler.
 *
 * @details
 *   LDMA_IRQHandler() is an ISR that fires at the end of every DMA transfer. The function enables TXC interrupts to
     alert the program that the string is finished sending and also disables the LEUART's ability to use DMA through
     the TXDMAWU macro.
 *****************************************************************************/
void LDMA_IRQHandler()
{
	CORE_ATOMIC_IRQ_DISABLE(); // Disable Processor interrupts at beginning of Handler
	int flag = (LDMA->IF & LDMA->IEN); // Get flag
	LDMA->IFC = flag; // Clear flag
	if(flag & (1 << TX_DMA_CHANNEL)) // Parse through flags
	{
		LEUART0->IEN |= LEUART_IEN_TXC; // Enable TXC interrupts
		LEUART0->CTRL &= ~LEUART_CTRL_TXDMAWU; // Disable DMA Wake up for TX
		while(LEUART0->SYNCBUSY); // Needed to sync HF clocks and LF clocks
	}
	if(flag & (1 << RX_DMA_CHANNEL)) // Parse through flags
	{
		//LEUART0->IEN |= LEUART_IEN_TXC; // Enable TXC interrupts
		LEUART0->CTRL &= ~LEUART_CTRL_RXDMAWU; // Disable DMA Wake up for TX
		while(LEUART0->SYNCBUSY); // Needed to sync HF clocks and LF clocks
	}

	CORE_ATOMIC_IRQ_ENABLE(); //Enable Processor interrupts at the beginning of Handler
}

