/*
 * APDS_9301_light_sensor.c
 *
 *  Created on: Mar 25, 2019
 *      Author: hardyk
 */

#include "../include/APDS_9301_light_sensor.h"

#define I2C_SLAVE_ADDRESS					(0x39)
#define SELECT_COMMAND_REGISTER				(0x80)
#define LOWER_THRESHOLD_VALUE				(0x64)
#define UPPER_THRESHOLD_VALUE				(0x4E20)

uint8_t extra_credit_light(int i2c_fd) {

	uint8_t ret = 0;
	uint16_t read_value = 0;

	/*Set Lower and Upper threshold value*/
	write_low_interrupt_threshold_register(i2c_fd, LOWER_THRESHOLD_VALUE);
	write_high_interrupt_threshold_register(i2c_fd, UPPER_THRESHOLD_VALUE);
	/*

	 uint16_t low_value = read_low_interrupt_threshold_register(i2c_fd);
	 uint16_t upper_value = read_high_interrupt_threshold_register(i2c_fd);

	 //printf("Light: low threshold and high threshold - %x and %x\n", low_value, upper_value);
	 */

	/*Enable interrupt*/
	enable_inerrupt(i2c_fd);

	/*Set Persist value*/
	set_interrupt_persitentancy(i2c_fd, 5);

	/*uint8_t read = read_interrupt_control_register(i2c_fd);
	 printf("Interrupt Controller - %x\n", read);
	 */




	return ret;
}

/**
 * @brief this function reads to the lower byte and uppper byte
 * of the low interrupt threshold register
 * @param i2c_fd
 * @return read threshold value
 */
uint16_t read_low_interrupt_threshold_register(int i2c_fd) {

	uint16_t low_threshold_value = 0;
	uint8_t msb_byte = 0, lsb_byte = 0, ret = 0;

	uint8_t command = SELECT_COMMAND_REGISTER | INTERRUPT_THRES_LOW_LOW;
	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &lsb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading low interrupt low threshold");
	}

	command = SELECT_COMMAND_REGISTER | INTERRUPT_THRES_LOW_HIGH;
	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &msb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading low interrupt high threshold");
	}

	low_threshold_value = (msb_byte << 8) | lsb_byte;

	return low_threshold_value;
}

/**
 * @brief this function writes lower byte and upper byte of
 * the low interrupt threshold register
 * @param i2c_fd
 * @param th_value requried threshold value to be written
 * @return no of bytes written
 */
uint8_t write_low_interrupt_threshold_register(int i2c_fd, uint16_t th_value) {

	uint8_t msb_byte = 0, lsb_byte = 0, ret = 0;

	msb_byte = (th_value & 0xFF00) >> 8;
	lsb_byte = (th_value & 0x00FF);

	uint8_t command = SELECT_COMMAND_REGISTER | INTERRUPT_THRES_LOW_LOW;
	write_command_register(i2c_fd, command);

	ret = write(i2c_fd, &lsb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: writing low interrupt low threshold");
	}

	command = SELECT_COMMAND_REGISTER | INTERRUPT_THRES_LOW_HIGH;
	write_command_register(i2c_fd, command);

	ret = write(i2c_fd, &msb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: writing low interrupt high threshold");
	}

	return ret;

}

/**
 * @brief this function writes lower byte and upper byte of
 * the high interrupt threshold register
 * @param i2c_fd
 * @param th_value requried threshold value to be written
 * @return no of bytes written
 */
uint8_t write_high_interrupt_threshold_register(int i2c_fd, uint16_t th_value) {

	uint8_t msb_byte = 0, lsb_byte = 0, ret = 0;

	msb_byte = (th_value & 0xFF00) >> 8;
	lsb_byte = (th_value & 0x00FF);

	uint8_t command = SELECT_COMMAND_REGISTER | INTERRUPT_THRES_HIGH_LOW;
	write_command_register(i2c_fd, command);

	ret = write(i2c_fd, &lsb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: writing low interrupt low threshold");
	}

	command = SELECT_COMMAND_REGISTER | INTERRUPT_THRES_HIGH_HIGH;
	write_command_register(i2c_fd, command);

	ret = write(i2c_fd, &msb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: writing low interrupt high threshold");
	}

	return ret;

}

/**
 * @brief this function reads to the lower byte and uppper byte
 * of the high interrupt threshold register
 * @param i2c_fd
 * @return read threshold value
 */
uint16_t read_high_interrupt_threshold_register(int i2c_fd) {

	uint16_t high_threshold_value = 0;
	uint8_t msb_byte = 0, lsb_byte = 0, ret = 0;

	uint8_t command = SELECT_COMMAND_REGISTER | INTERRUPT_THRES_HIGH_LOW;
	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &lsb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading low interrupt low threshold");
	}

	command = SELECT_COMMAND_REGISTER | INTERRUPT_THRES_HIGH_HIGH;
	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &msb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading low interrupt high threshold");
	}

	high_threshold_value = (msb_byte << 8) | lsb_byte;

	return high_threshold_value;
}

/**
 * @brief this function checks for the stae of day as day or night
 * @param lux_value
 * @return returns enum as calculated
 */
uint8_t day_or_night(double lux_value) {

	if (lux_value < 100) {
		return NIGHT;
	} else {
		return DAY;
	}
}

/**
 * @brief this function reads ADC0 channel value
 * @param i2c_fd
 * @return read value of the channel
 */
uint16_t read_data0_register(int i2c_fd) {

	uint8_t lsb_byte = 0, msb_byte = 0, ret = 0;
	uint16_t adc_channel0 = 0;

	uint8_t command = SELECT_COMMAND_REGISTER | DATA0_LOW_REGISTER;
	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &lsb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading data0 lower byte");
	}

	command = SELECT_COMMAND_REGISTER | DATA0_HIGH_REGISTER;
	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &msb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading data0 upper byte");
	}

	adc_channel0 = (msb_byte << 8) | lsb_byte;

	return adc_channel0;

}

/**
 * @brief this function reads ADC1 channel value
 * @param i2c_fd
 * @return read value of the cnannel
 */
uint16_t read_data1_register(int i2c_fd) {

	uint8_t lsb_byte = 0, msb_byte = 0, ret = 0;
	uint16_t adc_channel1 = 0;

	uint8_t command = SELECT_COMMAND_REGISTER | DATA1_LOW_REGISTER;
	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &lsb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading data0 lower byte");
	}

	command = SELECT_COMMAND_REGISTER | DATA1_HIGH_REGISTER;
	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &msb_byte, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading data0 upper byte");
	}

	adc_channel1 = (msb_byte << 8) | lsb_byte;

	return adc_channel1;

}

/**
 * @brief this function reads the ADC0 and ADC1 channel values and
 * converts the value into lux value
 * @param i2c_fd
 * @return lux value
 */
double read_lux_data(int i2c_fd) {

	uint16_t adc_channel1 = read_data1_register(i2c_fd);
	uint16_t adc_channle0 = read_data0_register(i2c_fd);

	//printf("ADC1 : %d, ADC0 : %d\n", adc_channel1, adc_channle0);

	double ratio = (double) adc_channel1 / (double) adc_channle0;
	double lux_value = 0;

	if (0 < ratio && ratio <= 0.50) {

		lux_value = (0.0304 * adc_channle0)
				- (0.062 * adc_channle0 * pow(ratio, 1.4));

	} else if (0.50 < ratio && ratio <= 0.61) {

		lux_value = (0.024 * adc_channle0) - (0.031 * adc_channel1);

	} else if (0.61 < ratio && ratio <= 0.80) {

		lux_value = (0.0128 * adc_channle0) - (0.0153 * adc_channel1);

	} else if (0.80 < ratio && ratio <= 1.30) {

		lux_value = (0.00146 * adc_channle0) - (0.00112 * adc_channel1);

	} else {

		lux_value = 0;
	}

	return lux_value;
}

/**
 * @brief this function reads the value of the parts and
 * silicon revision of that part number
 * @param i2c_fd
 * @return read value
 */
uint8_t read_id_register(int i2c_fd) {

	uint8_t read_data = 0, ret = 0;

	uint8_t command = SELECT_COMMAND_REGISTER | ID_REGISTER;

	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &read_data, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading ID register");
	}

	return read_data;
}

/**
 * @brief this function sets the value of the interrupt persitency
 * in the interrup control register
 * @param i2c_fd
 * @param persist required value
 * @return no of bytes written
 */
uint8_t set_interrupt_persitentancy(int i2c_fd, uint8_t persist) {

	uint8_t ret = 0;

	uint8_t data = read_interrupt_control_register(i2c_fd);

	data |= persist;

	ret = write_interrupt_control_register(i2c_fd, data);
	if (ret < 0) {
		perror("ERROR: setting interrupt persistency");
	}

	return ret;

}

/**
 * @brief this function clears interrupt
 * @param i2c_fd
 * @return
 */
uint8_t clear_interrupt(int i2c_fd) {

	uint8_t ret = 0;

	uint8_t command = 0xC0;

	ret = write_command_register(i2c_fd, command);

	return ret;
}

/**
 * @brief this function disables level interrupt output
 * @param i2c_fd
 * @return no of bytes written
 */

uint8_t disbale_interrupt(int i2c_fd) {

	uint8_t ret = 0;

	uint8_t data = read_interrupt_control_register(i2c_fd);

	data &= ~(1 << ICR_INTERRUPT_MASK);

	ret = write_interrupt_control_register(i2c_fd, data);
	if (ret < 0) {
		perror("ERROR: disabling interrupt");
	}

	return ret;
}

/**
 * @brief this function enabels leel interrupt output
 * @param i2c_fd
 * @return no of bytes written
 */
uint8_t enable_inerrupt(int i2c_fd) {

	uint8_t ret = 0;

	uint8_t data = read_interrupt_control_register(i2c_fd);

	data |= (1 << ICR_INTERRUPT_MASK);

	ret = write_interrupt_control_register(i2c_fd, data);
	if (ret < 0) {
		perror("ERROR: disabling interrupt");
	}

	return ret;
}

/**
 * @brief this function writes to the interrupt control register to set
 * the mode of interrupt and persistency of the interrupt
 * @param i2c_fd
 * @param data required mode and/or persistent value
 * @return no of bytes written
 */
uint8_t write_interrupt_control_register(int i2c_fd, uint8_t data) {

	uint8_t command = SELECT_COMMAND_REGISTER | INTERRUPT_REGISTER;
	uint8_t write_data = data, ret = 0;

	write_command_register(i2c_fd, command);

	ret = write(i2c_fd, &write_data, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: writing interrupt control register");
	}

	return ret;
}

/**
 * @brief this function reads the interrupt control registers
 * which reads mode of interrupt and interrupt persistence
 * @param i2c_fd
 * @return read value
 */
uint8_t read_interrupt_control_register(int i2c_fd) {

	uint8_t command = SELECT_COMMAND_REGISTER | INTERRUPT_REGISTER;
	uint8_t read_data = 0, ret = 0;

	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &read_data, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading interrupt control register");
	}

	return read_data;
}

/**
 * @brief this function sets integration time for ADC
 * @param i2c_fd
 * @param int_time requried integration time
 * @return number of bytes written
 */
uint8_t set_integration_time(int i2c_fd, uint8_t int_time) {

	uint8_t ret = write_timing_register(i2c_fd, int_time);
	if (ret < 0) {
		perror("ERROR: setting integration time");
	}
	return ret;
}

/**
 * @brief this function sets ADC gain to high 16x
 * @param i2c_fd
 * @return no of bytes written
 */
uint8_t set_gain_high(int i2c_fd) {

	uint8_t ret = write_timing_register(i2c_fd, SET_HIGH_GAIN);

	if (ret < 0) {
		perror("ERROR: setting gain to high");
	}

	return ret;
}

/**
 * @brief this function sets ADC gain to low 1x
 * @param i2c_fd
 * @return number of bytes written
 */
uint8_t set_gain_low(int i2c_fd) {

	uint8_t ret = write_timing_register(i2c_fd, SET_LOW_GAIN);

	if (ret < 0) {
		perror("ERROR: setting gain to low");
	}

	return ret;
}

/**
 * @brief this function reads timing register which is gain value and
 * integration time value for ADC
 * @param i2c_fd
 * @return read data
 */
uint8_t read_timing_register(int i2c_fd) {

	uint8_t command = SELECT_COMMAND_REGISTER | TIMING_REGISTER;
	uint8_t read_data = 0, ret = 0;

	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &read_data, NBYTES_1);
	if (ret < 0) {
		perror("reading timing register");
	}

	return read_data;

}

/**
 * @brief this function writes to the timing register which is used for
 * setting gain and integration time for ADC
 * @param i2c_fd
 * @param data required gain and integration time
 * @return no of bytes written
 */

uint8_t write_timing_register(int i2c_fd, uint8_t data) {

	uint8_t commnad = SELECT_COMMAND_REGISTER | TIMING_REGISTER;
	uint8_t ret = 0;
	uint8_t write_data = data;

	write_command_register(i2c_fd, commnad);

	ret = write(i2c_fd, &write_data, NBYTES_1);
	if (ret < 0) {
		perror("writing timing register");
	}

	return ret;
}

/**
 * @brief this function turns sensor OFF
 * @param i2c_fd
 * @return no of bytes written
 */
uint8_t power_up_sensor(int i2c_fd) {

	uint8_t ret = 0;

	ret = write_control_register(i2c_fd, POWER_ON_SENSOR);
	if (ret < 0) {
		perror("ERROR: power on sensor");
	}

	return ret;
}

/**
 * @brief this function turns the sensor OFF
 * @param i2c_fd
 * @return no of bytes written
 */
uint8_t power_down_sensor(int i2c_fd) {

	uint8_t ret = 0;

	ret = write_control_register(i2c_fd, POWER_OFF_SENSOR);
	if (ret < 0) {
		perror("ERROR: power off sensor");
	}

	return ret;
}

/**
 * @brief this function reads the control register and returns the read
 * value which is current state of the sensor ie ON or OFF
 * @param i2c_fd
 * @return read value
 */
uint8_t read_control_register(int i2c_fd) {

	uint8_t command = SELECT_COMMAND_REGISTER | CONTROL_REGISTER;
	uint8_t read_data = 0, ret = 0;

	write_command_register(i2c_fd, command);

	ret = read(i2c_fd, &read_data, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: reading control register");
	}

	return read_data;

}

/**
 * @brief this function writes to the control register which is used for powering the
 * sensor ON or OFF
 * @param i2c_fd
 * @param data on or off command
 * @return no of bytes writtern
 */
uint8_t write_control_register(int i2c_fd, uint8_t data) {

	uint8_t command = SELECT_COMMAND_REGISTER | CONTROL_REGISTER;

	write_command_register(i2c_fd, command);

	int ret = 0;
	int8_t write_data = data;

	ret = write(i2c_fd, &write_data, NBYTES_1);
	if (ret < 0) {
		perror("ERROR: writing to control register");
	}

	return ret;

}

/**
 * @brief this function writes to the commnad register
 * @param i2c_fd
 * @param command commnad to be written, usually it's the address of the register
 * going to be read or write ORed with 0x80
 * @return no of bytes written
 */
uint8_t write_command_register(int i2c_fd, uint8_t command) {

	uint8_t ret = 0;

	ret = write(i2c_fd, &command, NBYTES_1);

	if (ret < 0) {
		perror("ERROR: writing command register");
	}

	return ret;

}

/**
 * @brief this function initializes I2C module for light sensor
 * @return i2c module file descriptor
 */
int init_light_sensor(void) {

	int i2c_fd = 0;

	i2c_fd = open("/dev/i2c-2", O_RDWR);
	if (i2c_fd < 0) {
		perror("ERROR: i2c_open");
		exit(1);
	}

	if (ioctl(i2c_fd, I2C_SLAVE, I2C_SLAVE_ADDRESS) < 0) {
		perror("ERROR: i2c_ioctl");
		exit(1);
	}

	power_up_sensor(i2c_fd);
	int id = read_id_register(i2c_fd);
	printf("Id is %d", id);

	return i2c_fd;
}

