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

/*
 * i2c.c
 *
 *  Created on: Feb 8, 2019
 *      Author: Selasi Etchey
 */
#include "em_i2c.h"
#include "gpio.h"
#include "i2c.h"

/**************************************************************************//**
 * @brief Initializes/Configures the I2C Peripheral in master mode and enables the peripheral.
 *****************************************************************************/
void i2c_init()
{

/******************************I2C Struct********************************************************/
	I2C_Init_TypeDef myBus;
    myBus.enable = false;                  /* Enable when initialization done. */
    myBus.master = true;                  /* Set to master mode. */
    myBus.refFreq = 0;                    /* Use currently configured reference clock. */
    myBus.freq = I2C_FREQ_FAST_MAX; /* Set to standard rate assuring being */
    /*                        within I2C specification. */
    myBus.clhr =i2cClockHLRAsymetric;   /* Set to use 6:3 low/high duty cycle. */

    I2C_Init(I2C0,&myBus);

/************************************************************************************************/



/******************************Resetting SDA/SCL Pins********************************************************/
  /*  for(int i=0;i<9;i++)
    {
    	GPIO_PinOutClear(SCL_port, SCL_pin);
    	GPIO_PinOutSet(SCL_port, SCL_pin);
    }
    I2C0->CMD = I2C_CMD_ABORT;


    if(I2C0->STATE & I2C_STATE_BUSY)
    {
    	I2C0->CMD = I2C_CMD_ABORT;
    } */

/***********************************************************************************************************/

/******************************Routing SDA/SCL Line and Enabling Register Functions********************************************************/

    I2C0->ROUTELOC0 = (I2C_ROUTELOC0_SDALOC_LOC15 | I2C_ROUTELOC0_SCLLOC_LOC15);
    I2C0->ROUTEPEN |= (I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN);



    I2C0->CTRL |= (I2C_CTRL_CLTO_1024PCC | I2C_CTRL_BITO_160PCC);


    I2C0->IEN |= (I2C_IF_ACK | I2C_IF_RXDATAV);
   // I2C0->IFC |= I2C_IFC_CLTO | I2C_IFC_BITO;
   // I2C0->IEN |= I2C_IEN_CLTO | I2C_IEN_BITO;

    I2C_Enable(I2C0, true);

/******************************************************************************************************************************************/

}





