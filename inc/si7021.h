/**
 * @brief
 * si7021.h
 *
 *  Created on: 07.04.2021
 *      Author: Dominik Socher
 * 
 */

#ifndef SI7021_H_
#define SI7021_H_

#include "i2c.h"

/*
 * *****************************************************
 *
 * Public function prototypes
 *
 * *****************************************************
 */
///< Reset the sensor
extern void si7021_reset();
///< Init function for sensor
extern void si7021_init();
///< Read firmware from sensor
extern void si7021_read_firmware();
///< Read serial number from sensor
extern void si7021_read_serial_number();

///< Read Temperature from sensor
extern float si7021_read_temperature();
///< Read humidity from sensor
extern float si7021_read_humidity();

///< Read Si7021 user register
extern alt_u8 si7021_read_user_reg(alt_u8 reg);
///< Write toSi7021 user register
extern void si7021_write_user_reg(alt_u8 reg, alt_u8 value);

///< Activate the inbuilt heater
extern void si7021_heater(alt_u8 set);
///< Set the heater level
extern void si7021_heat_level(alt_u8 level);

#endif /* SI7021_H_ */
