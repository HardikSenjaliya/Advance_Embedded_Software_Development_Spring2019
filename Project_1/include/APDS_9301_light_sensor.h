/*
 * APDS_9301_light_sensor.h
 *
 *  Created on: Mar 25, 2019
 *      Author: hardyk
 */

#ifndef INCLUDE_APDS_9301_LIGHT_SENSOR_H_
#define INCLUDE_APDS_9301_LIGHT_SENSOR_H_

#include "../include/utils.h"

#include <math.h>
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

#define POWER_ON_SENSOR						(0x03)
#define POWER_OFF_SENSOR					(0x00)

#define INTEGRATION_TIME_13MS				(0x00)
#define INTEGRATION_TIME_101MS				(0x01)
#define INTEGRATION_TIME_402MS				(0x10)

#define SET_HIGH_GAIN						(0x01)
#define SET_LOW_GAIN						(0x00)


/*Register Address*/
#define CONTROL_REGISTER					(0x00)
#define TIMING_REGISTER						(0x01)
#define INTERRUPT_THRES_LOW_LOW				(0x02)
#define INTERRUPT_THRES_LOW_HIGH			(0x03)
#define INTERRUPT_THRES_HIGH_LOW			(0x04)
#define INTERRUPT_THRES_HIGH_HIGH			(0x05)
#define INTERRUPT_REGISTER					(0x06)
#define ID_REGISTER							(0x0A)
#define DATA0_LOW_REGISTER					(0x0C)
#define DATA0_HIGH_REGISTER					(0x0D)
#define DATA1_LOW_REGISTER					(0x0E)
#define DATA1_HIGH_REGISTER					(0x0F)

#define ICR_INTERRUPT_MASK					(0X04)



int init_light_sensor(void);
uint8_t write_command_register(int i2c_fd, uint8_t command);
uint8_t power_up_sensor(int i2c_fd);
uint8_t power_down_sensor(int i2c_fd);
uint8_t read_control_register(int i2c_fd);
uint8_t write_control_register(int i2c_fd, uint8_t data);
uint8_t read_timing_register(int i2c_fd);
uint8_t write_timing_register(int i2c_fd, uint8_t data);
uint8_t set_gain_high(int i2c_fd);
uint8_t set_gain_low(int i2c_fd);
uint8_t read_interrupt_control_register(int i2c_fd);
uint8_t write_interrupt_control_register(int i2c_fd, uint8_t data);
uint8_t enable_inerrupt(int i2c_fd);
uint8_t disbale_interrupt(int i2c_fd);
uint8_t clear_interrupt(int i2c_fd);
uint8_t set_interrupt_persitentancy(int i2c_fd, uint8_t persist);
uint8_t read_id_register(int i2c_fd);
uint16_t read_low_interrupt_threshold_register(int i2c_fd);
uint8_t write_low_interrupt_threshold_register(int i2c_fd, uint16_t th_value);
uint8_t write_high_interrupt_threshold_register(int i2c_fd, uint16_t th_value);
uint16_t read_high_interrupt_threshold_register(int i2c_fd);
uint16_t read_data0_register(int i2c_fd);
uint16_t read_data1_register(int i2c_fd);
double read_lux_data(int i2c_fd);
uint8_t day_or_night(double lux_value);
uint8_t extra_credit_light(int i2c_fd);


#endif /* INCLUDE_APDS_9301_LIGHT_SENSOR_H_ */
