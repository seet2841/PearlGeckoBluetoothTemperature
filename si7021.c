/***************************************************************************//**
 * @file si7021.c********************************************************************************
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
#include "gpio.h"
#include "si7021.h"
#include "sleep.h"
#include "letimer.h"

/**************************************************************************//**
 * @brief Reads temperature resolution number from the Temperature Sensor
 * @return temperature resolution
 *****************************************************************************/
uint8_t read_cmd()
{
	GPIO_PinOutSet(TEMP_SENSOR_port, TEMP_SENSOR_pin);
	I2C0->CMD = I2C_CMD_START;
	I2C0->TXDATA = (((SLAVE_ADDRESS) << 1)|WRITE_BIT);

	while(!(I2C0->IF & I2C_IF_ACK));
	I2C0->IFC = I2C_IFC_ACK;
	I2C0->TXDATA = READ_ADDRESS;

	while(!(I2C0->IF & I2C_IF_ACK));
	I2C0->IFC = I2C_IFC_ACK;
	I2C0->CMD = I2C_CMD_START;
	I2C0->TXDATA = (((SLAVE_ADDRESS) << 1)|READ_BIT);

	while(!(I2C0->IF & I2C_IF_ACK));
	I2C0->IFC = I2C_IFC_ACK;

	while(!(I2C0->IF & I2C_IF_RXDATAV));
	uint8_t data = I2C0->RXDATA;
	I2C0->CMD = I2C_CMD_NACK;
	I2C0->CMD = I2C_CMD_STOP;
	return data;
}
/**************************************************************************//**
 * @brief Writes command to Temp Sensor User Register 1
 * @param command: The command to write to the temp sensor (changing reoslution)
 *****************************************************************************/
void write_cmd(uint8_t command)
{
	GPIO_PinOutSet(TEMP_SENSOR_port, TEMP_SENSOR_pin);
	I2C0->CMD = I2C_CMD_START;
	I2C0->TXDATA = (((SLAVE_ADDRESS) << 1)|WRITE_BIT);

	while(!(I2C0->IF & I2C_IF_ACK));
	I2C0->IFC = I2C_IFC_ACK;
	I2C0->TXDATA = WRITE_ADDRESS;

	while(!(I2C0->IF & I2C_IF_ACK));
	I2C0->IFC = I2C_IFC_ACK;
	I2C0->TXDATA = command;

	I2C0->CMD = I2C_CMD_ACK;
	I2C0->CMD = I2C_CMD_STOP;
}

/**************************************************************************//**
 * @brief Reads temperature code from temperature sensor
 * @return 16 bit temperature code
 *****************************************************************************/
uint16_t temp_read()
{
	I2C0->CMD = I2C_CMD_ABORT;
	I2C0->CMD = I2C_CMD_CLEARPC;

	GPIO_PinOutSet(TEMP_SENSOR_port, TEMP_SENSOR_pin);

	I2C0->CMD = I2C_CMD_START;
	I2C0->TXDATA = (((SLAVE_ADDRESS) << 1) | WRITE_BIT);
	while(!(I2C0->IF & I2C_IF_ACK));
	I2C0->IFC = I2C_IFC_ACK;


	I2C0->TXDATA = MEASURE_CMD;
	while(!(I2C0->IF & I2C_IF_ACK));
	I2C0->IFC = I2C_IFC_ACK;
	I2C0->CMD = I2C_CMD_START;


	I2C0->TXDATA = (((SLAVE_ADDRESS) << 1) | READ_BIT);
	while(!(I2C0->IF & I2C_IF_ACK));
	I2C0->IFC = I2C_IFC_ACK;

	while(!(I2C0->IF & I2C_IF_RXDATAV));
	uint16_t MS_BYTE = I2C0->RXDATA;
	I2C0->CMD = I2C_CMD_ACK;

	while(!(I2C0->IF & I2C_IF_RXDATAV));
	uint16_t LS_BYTE = I2C0->RXDATA;
	I2C0->CMD = I2C_CMD_NACK;
	I2C0->CMD = I2C_CMD_STOP;

	MS_BYTE = (MS_BYTE << 8);
	uint16_t TEMP_BYTES = (MS_BYTE | LS_BYTE);
	return TEMP_BYTES;
}

/**************************************************************************//**
 * @brief Converts 16 bit temp code from temp sensor to degrees celcius
 * @return float of temperature code in degrees celcius
 *****************************************************************************/
float convert_tempCel(uint16_t data)
{
	float temp = ((data*175.72)/(65536)) - 46.85;
	return temp;
}

/**************************************************************************//**
 * @brief Enables load power management by blocking sleep mode (EM3), and setting up I2C pins
 *****************************************************************************/
void Enable_LPM(void)
{
	blockSleepMode(EM3);
	GPIO_PinModeSet(SCL_port,SCL_pin,gpioModeWiredAnd,SDA_default);
	GPIO_PinModeSet(SDA_port,SDA_pin,gpioModeWiredAnd,SCL_default);
    for(int i=0;i<9;i++)
    {
    	GPIO_PinOutClear(SCL_port, SCL_pin);
    	GPIO_PinOutSet(SCL_port, SCL_pin);
    }
    I2C0->CMD = I2C_CMD_ABORT;
    if(I2C0->STATE & I2C_STATE_BUSY)
    {
    	I2C0->CMD = I2C_CMD_ABORT;
    }
}

/**************************************************************************//**
 * @brief Disabled load power management by disabling I2C pins, turning off the temperature sensor
 *        and unblocking the sleepmode
 *****************************************************************************/
void Disable_LPM(void)
{
	GPIO_PinModeSet(SCL_port,SCL_pin,gpioModeDisabled,SDA_default);
	GPIO_PinModeSet(SDA_port,SDA_pin,gpioModeDisabled,SCL_default);
	GPIO_PinOutClear(TEMP_SENSOR_port,TEMP_SENSOR_pin);
	unblockSleepMode(EM3);
}

