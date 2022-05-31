/*
 * si7021.c
 *
 *  Created on: 07.04.2021
 *      Author: Dominik
 *
 *      Functions to drive si7021 temperature sensor via i2c
 */

#include "si7021.h"

/*
 * *********************************************************
 *              Constant 
 *              See datasheet page 18
 * *********************************************************
 */

///<device base address
#define SI7021_ADDRESS 0x40 
///<Measure Relative Humidity, Hold Master Mode 
#define SI7021_MEASRH_HOLD  0xE5 
///<Measure Relative Humidity, No Hold Master Mode
#define SI7021_MEASRH_NOHOLD 0xF5 
//<Measure Temperature, Hold Master Mode
#define SI7021_MEASTEMP_HOLD 0xE3 
///< Measure Temperature, No Hold Master Mode
#define SI7021_MEASTEMP_NOHOLD 0xF3 
///<Read Temperature Value from Previous RH Measurement
#define SI7021_READPREVTEMP 0xE0 
//</Reset Command 
#define SI7021_RESET 0xFE
///<Write RH/T User Register 1 
#define SI7021_WRITERHT_REG 0xE6
//Read RH/T User Register 1     
#define SI7021_READRHT_REG 0xE7
///<Write Heater Control Register 
#define SI7021_WRITEHEATER_REG 0x51 
///<Read Heater Control Register 
#define SI7021_READHEATER_REG 0x11 
///<Control Register Heater Bit 
#define SI7021_REG_HTRE_BIT 0x02 
///<Read Electronic ID 1. Byte   
#define SI7021_ID11 0xFA 
#define SI7021_ID12 0x0F 
///<Read Electronic ID 2. Byte 
#define SI7021_ID21 0xFC
#define SI7021_ID22 0xC9
///<Read Firmware Revision 
#define SI7021_FIRMVERS_A 0x84     
#define SI7021_FIRMVERS_B 0xB8  
///<Sensor revision 1 
#define SI7021_REV_1 0xff 
///<Sensor revision 2 
#define SI7021_REV_2 0x20 

///<global buffer to hold tx/rx
alt_u8 txBuffer[3];
alt_u8 rxBuffer[3];
///<firmware revision number
alt_u8 firmware_revsion = 0;

///<sensor model
alt_u8 model = 0;

/**
 * *********************************************************
 * @brief
 * si7021_read_user_reg();
 *
 * Read from the si7021 user register 
 * @param[in] register Register to write
 * @param[out] value
 * @return Retruns the value of the user Register. After reset it holds  0011_1010
 *
 * *********************************************************
 */
alt_u8 si7021_read_user_reg(alt_u8 reg)
{
	txBuffer[0] = reg;

	i2c_tx(1, txBuffer);
	//wait a while in between transactions
	i2c_rx(1, rxBuffer);

	alt_u8 value = rxBuffer[0];

	return value;
}

/**
 * *********************************************************
 * @brief
 * si7021_write_user_reg();
 *
 * Write to the si7021 user register 
 * @param[in] register Register to write
 * @param[out] value Register value to write
 * @param[out] void
 *
 * *********************************************************
 */
void si7021_write_user_reg(alt_u8 reg, alt_u8 value)
{
	txBuffer[0] = reg;
	txBuffer[1] = value;
	i2c_tx(2, txBuffer);
}

/**
 * *********************************************************
 * @brief
 * void si7021_reset();
 *
 * Reset function for si7021 
 * @param[in] none
 * @param[out] void
 *
 * *********************************************************
 */
void si7021_reset()
{
	txBuffer[0] = SI7021_RESET;
	//send data to si7021
	i2c_tx(1, txBuffer);
}

/**
 * *********************************************************
 * @brief
 * void si7021_init();
 *
 * Init function for si7021 needs to be called first in order for the sensor to work. 
 * @param[in] none
 * @param[out] void
 *
 * *********************************************************
 */
void si7021_init()
{
	//open i2c port
	i2c_open();
	//set address
	i2c_slave_address(SI7021_ADDRESS);
	//reset device
	si7021_reset();
	si7021_heat_level(1);
}

/**
 * *********************************************************
 * @brief
 * si7021_read_humidity();
 *
 * Reads humidity from si7021 and in No Hold Master Mode. Retruns humidity in %RH as a 16 bit integer
 *  @param[in] none
 * @param[out] humidity 16 bit value 
 * @return Functions returns the calculated humidity value in percent as a float
 *
 * *********************************************************
 */
float si7021_read_humidity()
{
	alt_u16 temp = 0;
	alt_u8 checksum = 0;
	txBuffer[0] = SI7021_MEASRH_NOHOLD;
	//send data to si7021
	i2c_tx(1, txBuffer);
	//returns humidity as 16 bit value plus checksum
	i2c_rx(2, rxBuffer);
	temp = rxBuffer[0] << 8 | rxBuffer[1];
	checksum = rxBuffer[3];

	//calculate actual humidity
	float humidity = temp;
	humidity *= 125;
	humidity /= 65536;
	humidity -= 6;
	return humidity;
}
/**
 * *********************************************************
 * @brief
 * void si7021_read_temperature()
 *
 * Reads temperature from si7021 and in No Hold Master Mode. Retruns it in celsius as a 16 bit integer.
 *  @param[in] none
 * @param[out] temperature 16 bit value 
 * @return Functions returns the calculated temperature value in celsius as a float.
 *
 * *********************************************************
 */
float si7021_read_temperature()
{
	float temp = 0;
	alt_u8 checksum = 0;
	txBuffer[0] = SI7021_MEASTEMP_NOHOLD;
	//send data to si7021
	i2c_tx(1, txBuffer);
	//returns humidity as 16 bit value plus checksum
	i2c_rx(2, rxBuffer);
	temp = rxBuffer[0] << 8 | rxBuffer[1];
	checksum = rxBuffer[3];

	//calculate actual celsius
	float temperature = temp;
	temperature *= 175.72;
	temperature /= 65536;
	temperature -= 46.85;

	return temperature;
}

/**
 * *********************************************************
 * @brief
 * si7021_read_firmware();
 *
 * Send 2 byte instruction data to the device and returns value 1 or 2 according to firmware revsion
 *              0xFF version 1.0
 *              0x20 version 2.0
 *              
 * @param[in] none
 * @param[out] void
 * *********************************************************
 */
extern void si7021_read_firmware()
{
	alt_u8 temp = 0;
	txBuffer[0] = SI7021_FIRMVERS_A;
	txBuffer[1] = SI7021_FIRMVERS_B;
	//send data to si7021
	i2c_tx(2, txBuffer);

	i2c_rx(1, rxBuffer);
	temp = rxBuffer[0];

	//check firmware revsion number and save to variable
	if (temp == SI7021_REV_1)
	{
		firmware_revsion = 1;
	}
	else if (temp == SI7021_REV_2)
	{
		firmware_revsion = 2;
	}
	else
	{
		firmware_revsion = 0;
	}
}

/**
 * *********************************************************
 * @brief
 * si7021_read_serial_number();
 *
 *  Function read the serial number. Device 
 *              returns 64 bit value where of 
 *              0x00 or 0xFF are engineering samples
 *              0x0D = Si7013
 *              0x14 = Si7020
 *              0x15 = Si7021 
 *              Two I2C commands are required to  access the device memory 
 *              and retrieve the complete serial number.
 *              see datasheet page 23
 * @param[in] none
 * @param[out] void
 *
 * *********************************************************
 */
extern void si7021_read_serial_number()
{
	//electronic serial number
	alt_u32 number_a = 0;
	alt_u32 number_b = 0;

	//first i2c transaction
	txBuffer[0] = SI7021_ID11;
	txBuffer[1] = SI7021_ID12;
	//send data to si7021
	i2c_tx(2, txBuffer);

	i2c_rx(3, rxBuffer);

	number_a = rxBuffer[0] << 8 | rxBuffer[1] << 8 | rxBuffer[2] << 8
			| rxBuffer[3];


	//first i2c transaction
	txBuffer[0] = SI7021_ID21;
	txBuffer[1] = SI7021_ID22;
	//send data to si7021
	i2c_tx(2, txBuffer);

	i2c_rx(3, rxBuffer);

	number_b = rxBuffer[0] << 8 | rxBuffer[1] << 8 | rxBuffer[2] << 8
			| rxBuffer[3];

	//Determine model
	//and shift number b
	switch (number_b >> 24)
	{
	case 0:
	case 0xff:
		//engineering samples
		model = 1;
		break;
	case 0x0d:
		//Si7013
		model = 2;
		break;
	case 0x14:
		//Si7020
		model = 3;
		break;
	case 0x15:
		//Si7021
		model = 4;
		break;
	default:
		//no model
		model = 5;
		break;
	}
}

/**
 * *********************************************************
 * @brief
 * si7021_heater();
 *
 * Activate the sensor heater
 *              
 * @param[in] set When value hold 1 heater gets activated, when 0 heater off.
 * @param[out] void
 *
 * *********************************************************
 */
void si7021_heater(alt_u8 set)
{
	alt_u8 value = si7021_read_user_reg(SI7021_READRHT_REG);

	if (set == 1)
	{
		//set bit 2 in user register for heater
		value |= (1 << (SI7021_REG_HTRE_BIT));
	}
	else
	{
		//reset bit 1 in user register
		value &= ~(1 << (SI7021_REG_HTRE_BIT));
	}
	si7021_write_user_reg(SI7021_WRITERHT_REG, value);
	// printf("%d", value);
}

/**
 * *********************************************************
 * @brief
 * si7021_heater_level();
 *
 * Description: Sets the level of the internal heater
 *              At VDD 3.3 V
 *              current is 
 *              level       typical current draw
 *              0000        3.09 mA
 *              0001        9.18 mA
 *              0010        15.24 mA
 *              ....         ....
 *              0100        27.39 mA
 *              ....         ....       
 *              1000        51.69 mA
 *              ....         ....
 *              1111        94.20mA
 * @param[in] level sees above
 * @param[out] void
 *
 * *********************************************************
 */
void si7021_heat_level(alt_u8 level)
{
	si7021_write_user_reg(SI7021_WRITEHEATER_REG, level);
}

