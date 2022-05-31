/*
 * i2c.c
 *
 *  Created on: 01.04.2021
 *      Author: Dominik Socher
 *
 *      Functions for i2c communications.
 *      Needs altera avalon i2c master
 */

#include "i2c.h"

static struct
{
	// Pointer to Avalon i2c instance structure
	ALT_AVALON_I2C_DEV_t *i2c_dev;
	// Variable to hold status codes
	ALT_AVALON_I2C_STATUS_CODE i2c_status;
} i2c;

/**
 * *********************************************************
 * @brief
 * i2c_open()
 *
 * Function activates i2c device
 *  @param[in] none
 * @param[out] void
 *
 * *********************************************************
 */
void i2c_open(void)
{
	i2c.i2c_dev = alt_avalon_i2c_open("/dev/i2c_0");
	if (!i2c.i2c_dev)
	{
		printf("Error: Cannot find /dev/i2c_0\n");
	}
}

/**
 * *********************************************************
 * @brief
 * i2c_slave_address()
 *
 *  Set address of I2C slave
 *  @param[in] slave_address Address of the i2c slave
 * @param[out] none
 *
 * *********************************************************
 */
void i2c_slave_address(alt_u8 slave_address)
{
	alt_avalon_i2c_master_target_set(i2c.i2c_dev, slave_address);
}

/**
 * *********************************************************
 * @brief
 * i2c_tx()
 *
 * Set data to i2c slave, Data to send needs to be an array. If only one byte to send data[0]
 * @param[in] bytes Ammount of bytes in the transmit array
 * @param[in] data Data to transmit
 * @param[out] none
 *
 * *********************************************************
 */
void i2c_tx(alt_u8 bytes, alt_u8 *data)
{
	i2c.i2c_status = alt_avalon_i2c_master_tx(i2c.i2c_dev, data, bytes,
			ALT_AVALON_I2C_NO_INTERRUPTS);
	//when not i2c status exit function
	if (i2c.i2c_status != ALT_AVALON_I2C_SUCCESS)
	{
		printf("fail to send data\n");
	}
}

/**
 * *********************************************************
 * @brief
 * i2c_rx()
 *
 * Receive data from i2c slave. Data to receive needs to be an array. If only one byte to receive data[0]
 * @param[in] bytes Ammount of bytes in the receive array
 * @param[in] data Data to receive
 * @param[out] none
 *
 * *********************************************************
 */
void i2c_rx(alt_u8 bytes, alt_u8 *data)
{
	i2c.i2c_status = alt_avalon_i2c_master_rx(i2c.i2c_dev, data, bytes,
			ALT_AVALON_I2C_NO_INTERRUPTS);
	//when not i2c status exit function
	if (i2c.i2c_status != ALT_AVALON_I2C_SUCCESS)
	{
		printf("fail to receive data\n");
	}
}
