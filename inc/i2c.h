/**
 * @brief
 * i2c.h
 *
 *  Created on: 01.04.2021
 *      Author: Dominik Socher
 *  Software definitions fot altera avalon i2c master
 */

#ifndef I2C_H_
#define I2C_H_

#include <alt_types.h>
#include <altera_avalon_i2c.h>
#include <io.h>
#include <system.h>
#include <stdio.h>

/*
 * *****************************************************
 *
 * Public function prototypes
 *
 * *****************************************************
 */
///<Open is communication
extern void i2c_open(void);
///<Choose slave address
extern void i2c_slave_address(alt_u8 slave_address);
///< Transmit data to i2c slave
extern void i2c_tx(alt_u8 bytes, alt_u8 *data);
///< Receive data from i2c slave
extern void i2c_rx(alt_u8 bytes, alt_u8 *data);

#endif /* I2C_H_ */
