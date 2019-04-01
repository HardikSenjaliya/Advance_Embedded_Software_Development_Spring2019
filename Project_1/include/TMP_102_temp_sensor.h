/*
 * TMP_102_temp_sensor.h
 *  @brief header file for the temperature sensor i2c driver
 *
 *  Created on: Mar 25, 2019
 *      Author: hardyk
 */

#ifndef INCLUDE_TMP_102_TEMP_SENSOR_H_
#define INCLUDE_TMP_102_TEMP_SENSOR_H_

#include "../include/utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>


#define NEGATIVE_CHECK						(0x10000000)

#define TEMPERATURE_REGISTER 				(0)
#define CONFIGURATION_REGISTER				(1)
#define TLOW_REGISTER						(2)
#define THIGH_REGISTER						(3)

#define CR_EXTENDED_MODE_MASK				(12)
#define CR_ALERT_PIN_MASK					(13)
#define CR_CONVERSION_RATE_MASK				(14)
#define CR_RESOLUTION_MASK					(5)
#define CR_SHUTDOWN_MODE_MASK				(0)
#define CR_THERMOSTAT_MASK					(1)
#define CR_FAULT_BIT_MASK					(3)

#define FAULTS_ONE							(0)
#define FAULTS_TWO							(1)
#define FAULTS_FOUR							(2)
#define FAULTS_SIX							(3)

#define CONVERSION_RATE_QUARTER_HZ			(0)
#define CONVERSION_RATE_ONE_HZ				(1)
#define CONVERSION_RATE_DEFAULT				(2)
#define CONVERSION_RATE_EIGHT_HZ			(3)

#define COMPARATOR_MODE						(0)
#define INTERRUPT_MODE						(1)

#define DISBALE_SHUTDOWN_MODE				(0)
#define ENABLE_SHUTDOWN_MODE				(1)

#define NORMAL_MODE_OPERATION				(0)
#define EXTENDED_MODE_OPERATION				(1)

#define TLOW_VALUE							(25)
#define THIGH_VALUE							(31)

/*Function Prototypes*/
int init_temp_sensor(void);
double read_temperature_register(int i2c_fd);
double convert_temp_kelvin(int i2c_fd);
double convert_temp_farenheit(int i2c_fd);
double binary_to_decimal(uint8_t binary_data[]);
uint16_t read_config_register(int i2c_fd);
uint8_t write_pointer_register(int i2c_fd, uint8_t reg);
uint8_t write_config_register(int i2c_fd, uint16_t data);
uint8_t configure_shutdown_mode(int i2c_fd, bool required_mode);
uint8_t configure_sensor_resolution(int i2c_fd);
uint8_t configure_EM_operation(int i2c_fd, bool required_operation);
uint8_t configure_conversion_rate(int i2c_fd, uint8_t required_rate);
uint8_t configure_thermostate_mode(int i2c_fd, bool required_mode);
uint8_t configure_fault_bits(int i2c_fd, uint8_t required_faults);
uint8_t write_thigh_register(int i2c_fd, uint16_t t_value);
uint8_t write_tlow_register(int i2c_fd, uint16_t t_value);
uint16_t read_thigh_register(int i2c_fd);
uint16_t read_tlow_register(int i2c_fd);
uint8_t extra_credit_temp(int i2c_fd);



#endif /* INCLUDE_TMP_102_TEMP_SENSOR_H_ */
