/*
 * TMP_102_temp_sensor.h
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

#define NBYTES_1							(0x01)
#define NBYTES_2							(0x02)
#define NBYTES_3							(0x03)
#define NEGATIVE_CHECK						(0x10000000)

#define TEMPERATURE_REGISTER 				(0x00)
#define CONFIGURATION_REGISTER				(0x01)
#define TLOW_REGISTER						(0x02)
#define THIGH_REGISTER						(0x03)

#define CR_EXTENDED_MODE_MASK				(0x12)
#define CR_ALERT_PIN_MASK					(0x13)
#define CR_CONVERSION_RATE_MASK				(0x14)
#define CR_RESOLUTION_MASK					(0x05)
#define CR_SHUTDOWN_MODE_MASK				(0x00)
#define CR_THERMOSTAT_MASK					(0X01)
#define CR_FAULT_BIT_MASK					(0x03)

#define FAULTS_ONE							(0x00)
#define FAULTS_TWO							(0x01)
#define FAULTS_FOUR							(0x02)
#define FAULTS_SIX							(0x03)

#define CONVERSION_RATE_QUARTER_HZ			(0x00)
#define CONVERSION_RATE_ONE_HZ				(0x01)
#define CONVERSION_RATE_DEFAULT				(0x02)
#define CONVERSION_RATE_EIGHT_HZ			(0x03)

#define COMPARATOR_MODE						(0x00)
#define INTERRUPT_MODE						(0x01)

#define DISBALE_SHUTDOWN_MODE				(0X00)
#define ENABLE_SHUTDOWN_MODE				(0x01)

#define NORMAL_MODE_OPERATION				(0x00)
#define EXTENDED_MODE_OPERATION				(0x01)


/*Function Prototypes*/
int init_temp_sensor(void);
double read_temperature_register(int i2c_fd);
double convert_temp_kelvin(int i2c_fd);
double convert_temp_farenheit(int i2c_fd);
double binary_to_decimal(int8_t binary_data[]);
int16_t read_config_register(int i2c_fd);
int8_t write_pointer_register(int i2c_fd, int8_t reg);
int8_t write_config_register(int i2c_fd, int16_t data);
int8_t configure_shutdown_mode(int i2c_fd, bool required_mode);
int8_t configure_sensor_resolution(int i2c_fd);
int8_t configure_EM_operation(int i2c_fd, bool required_operation);
int8_t configure_conversion_rate(int i2c_fd, int8_t required_rate);
int8_t configure_thermostate_mode(int i2c_fd, bool required_mode);
int8_t configure_fault_bits(int i2c_fd, int8_t required_faults);
int8_t write_thigh_register(int i2c_fd, int16_t t_value);
int8_t write_tlow_register(int i2c_fd, int16_t t_value);
int16_t read_thigh_register(int i2c_fd);
int16_t read_tlow_register(int i2c_fd);



#endif /* INCLUDE_TMP_102_TEMP_SENSOR_H_ */
