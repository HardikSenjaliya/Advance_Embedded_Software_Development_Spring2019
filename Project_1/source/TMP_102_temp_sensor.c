/*
 * TMP_102_temp_sensor.c
 *
 *  Created on: Mar 25, 2019
 *      Author: hardyk
 */

#include "../include/TMP_102_temp_sensor.h"

#define I2C_SLAVE_ADDRESS				(0x48)
#define CONFIG_REG_DEFAULT				(0xA060) /*TODO check for the correctness of the MSB and LSB order*/

/**
 * @brief this functino reads the temperature value of the TLOW register
 * @param i2c_fd
 * @return tlow temperature value
 */
int16_t read_tlow_register(int i2c_fd) {

	int8_t read_Buffer[NBYTES_2];
	int8_t read_bytes = 0;
	double read_temp = 0;

	if (write_pointer_register(i2c_fd, TLOW_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	read_bytes = read(i2c_fd, &read_Buffer, NBYTES_2);
	if (read_bytes < 0) {
		perror("ERROR: reading tlow register");
	}

	read_temp = binary_to_decimal(read_Buffer);

	return read_temp;
}

/**
 * @brief this function writes the required value of the tlow
 * @param i2c_fd
 * @param t_value value to be written into the register
 * @return no of bytes written
 */
int8_t write_tlow_register(int i2c_fd, int16_t t_value) {

	int8_t ret = 0;

	if (write_pointer_register(i2c_fd, TLOW_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	int8_t write_buffer[NBYTES_3];
	write_buffer[0] = CONFIGURATION_REGISTER;
	write_buffer[1] = (t_value >> 8);
	write_buffer[2] = (t_value & 0xFF);

	ret = write(i2c_fd, &write_buffer, NBYTES_3);
	if (ret < 0) {
		perror("ERROR: while writing data to configuration register\n");
	}

	return ret;
}


/**
 * @brief this function reads the value of the THIGH register
 * @param i2c_fd
 * @return thigh temp value
 */
int16_t read_thigh_register(int i2c_fd) {

	int8_t read_Buffer[NBYTES_2];
	int8_t read_bytes = 0;
	double read_temp = 0;

	if (write_pointer_register(i2c_fd, THIGH_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	read_bytes = read(i2c_fd, &read_Buffer, NBYTES_2);
	if (read_bytes < 0) {
		perror("ERROR: reading tlow register");
	}

	read_temp = binary_to_decimal(read_Buffer);

	return read_temp;
}

/**
 * @brief this function writes requrired thigh value into the register
 * @param i2c_fd
 * @param t_value required thigh value
 * @return no of bytes written
 */

int8_t write_thigh_register(int i2c_fd, int16_t t_value) {

	int8_t ret = 0;

	if (write_pointer_register(i2c_fd, THIGH_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	int8_t write_buffer[NBYTES_3];
	write_buffer[0] = CONFIGURATION_REGISTER;
	write_buffer[1] = (t_value >> 8);
	write_buffer[2] = (t_value & 0xFF);

	ret = write(i2c_fd, &write_buffer, NBYTES_3);
	if (ret < 0) {
		perror("ERROR: while writing data to configuration register\n");
	}

	return ret;
}

/**
 * @brief this function sets the mode of opearation either comparator or interrupt mode
 * @param i2c_fd file descriptor of the opened i2c module
 * @param required_mode
 * @return no of bytes written
 */
int8_t configure_thermostate_mode(int i2c_fd, bool required_mode) {

	int16_t current_config = 0;
	int ret = 0;

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	current_config = read_config_register(i2c_fd);

	if (required_mode == INTERRUPT_MODE) {
		current_config |= (1 << CR_THERMOSTAT_MASK);
	} else {
		current_config &= ~(1 << CR_THERMOSTAT_MASK);
	}

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	ret = write_config_register(i2c_fd, current_config);
	if (ret < 0) {
		perror("ERROR: while setting shutdown mode");
	}

	return ret;
}

/**
 * @brief this function sets the requied number of consecutive faults bits
 * 			configurable into four modes - 1, 2, 4 and 6
 * @param i2c_fd file descriptor of the opened i2c module
 * @param required_faults required number of faults bits
 * @return no of bytes written
 */
int8_t configure_fault_bits(int i2c_fd, int8_t required_faults) {

	int16_t current_config = 0;
	int ret = 0;

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	current_config = read_config_register(i2c_fd);

	current_config |= (required_faults << CR_FAULT_BIT_MASK);

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	ret = write_config_register(i2c_fd, current_config);
	if (ret < 0) {
		perror("ERROR: while setting shutdown mode");
	}

	return ret;

}
/**
 * @brief this function sets the conversion rate of the sensor
 *		 	configurable into four modes - 0.25Hz, 1 Hz, 4 Hz, 8 Hz
 * @param i2c_fd file descriptor of the opened i2c module
 * @param required_rate conversion rate to be configured
 * @return no of bytes written
 */
int8_t configure_conversion_rate(int i2c_fd, int8_t required_rate) {

	int16_t current_config = 0;
	int ret = 0;

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	current_config = read_config_register(i2c_fd);

	current_config |= (required_rate << CR_CONVERSION_RATE_MASK);

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	ret = write_config_register(i2c_fd, current_config);
	if (ret < 0) {
		perror("ERROR: while setting shutdown mode");
	}

	return ret;

}

/**
 * @brief this function configures extended mode of operation of the sensor
 * @param i2c_fd file descriptor of the opened i2c module
 * @param required_operation 1 = extended mode, 0 = normal mode
 * @return no of bytes written
 */
int8_t configure_EM_operation(int i2c_fd, bool required_operation) {

	int16_t current_config = 0;
	int ret = 0;

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	current_config = read_config_register(i2c_fd);

	if (required_operation == EXTENDED_MODE_OPERATION) {
		current_config |= (1 << CR_EXTENDED_MODE_MASK);
	} else {
		current_config &= ~(1 << CR_EXTENDED_MODE_MASK);
	}

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	ret = write_config_register(i2c_fd, current_config);
	if (ret < 0) {
		perror("ERROR: while setting shutdown mode");
	}

	return ret;
}

/**
 * @brief this function configures/reads the resolution of the sensor
 * sensor resolution is read only setting.
 * @param i2c_fd file descriptor of the opened i2c module
 * @param requied_resolution
 * @return read resolution value
 */

int8_t configure_sensor_resolution(int i2c_fd) {

	int16_t read_data = 0;
	int8_t resolution = 0;
	int8_t msb = 0; //lsb = 0;

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	read_data = read_config_register(i2c_fd);

	msb = (read_data >> 8);
	//lsb = (read_data & 0xFF);

	resolution = (msb & 0x60);

	return resolution;
}

/**
 * @brief this function enables/disables sensor's mode of operation
 * 			1 = enables shutdown mode, 0 disables shutdown mode
 * @param i2c_fd file descriptor of the opened i2c module
 * @param required_mode
 * @return no of bytes written
 */
int8_t configure_shutdown_mode(int i2c_fd, bool required_mode) {

	int16_t current_config = 0;
	int ret = 0;

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	current_config = read_config_register(i2c_fd);

	current_config |= (required_mode << CR_SHUTDOWN_MODE_MASK);

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	ret = write_config_register(i2c_fd, current_config);
	if (ret < 0) {
		perror("ERROR: while setting shutdown mode");
	}

	return ret;
}

int8_t write_config_register(int i2c_fd, int16_t data) {

	int8_t ret = 0;

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		perror("ERROR: while writing to the pointer register\n");
	}

	int8_t write_buffer[NBYTES_3];
	write_buffer[0] = CONFIGURATION_REGISTER;
	write_buffer[1] = (data >> 8);
	write_buffer[2] = (data & 0xFF);

	ret = write(i2c_fd, &write_buffer, NBYTES_3);
	if (ret < 0) {
		perror("ERROR: while writing data to configuration register\n");
	}

	return ret;
}

/**
 * @brief this function reads the Configuration Register
 * @param i2c_fd file descriptor of the opened i2c module
 * @return read value
 */
int16_t read_config_register(int i2c_fd) {

	int16_t read_buffer = 0;
	int read_bytes = 0;

	if (write_pointer_register(i2c_fd, CONFIGURATION_REGISTER)) {
		ERROR_STDOUT("ERROR: while writing to the pointer register\n");
	}

	read_bytes = read(i2c_fd, &read_buffer, NBYTES_2);
	if (read_bytes < 0) {
		perror("ERROR: reading configuration register");
	}

	return read_buffer;
}

/**
 * @brief this function returns temperature in kelvin unit
 * @param i2c_fd
 * @return temperature in degree kelvin
 */
double convert_temp_kelvin(int i2c_fd) {

	double temp_k = read_temperature_register(i2c_fd);

	temp_k += 273.5;

	return temp_k;
}

/**
 * @brief this function returns temperature in farenheit unit
 * @param i2c_fd
 * @return tempearature in degree farenheit
 */
double convert_temp_farenheit(int i2c_fd) {

	double temp_f = read_temperature_register(i2c_fd);

	temp_f = temp_f * 1.8;
	temp_f += 32;

	return temp_f;
}

/**
 * @brief this function converts digital(binary) temp data read from the register
 * into decimal value
 * @param binary_data
 * @return decimal temperature value
 */
double binary_to_decimal(int8_t binary_data[]) {

	double decimal_temp = 0;
	int16_t temp = 0;

	int8_t msb_byte = binary_data[0];
	int8_t lsb_byte = binary_data[1];

	temp = ((((uint16_t) msb_byte) << 4) | (lsb_byte >> 4)) & 0xFFF;

	if (msb_byte & NEGATIVE_CHECK) {

		decimal_temp = ((~temp + 1) * 0.0625) * (-1);

	} else {

		decimal_temp = temp * 0.0625;
	}

	return decimal_temp;
}

/**
 * @brief this function reads the temperature value in binary format from the
 * register
 * @param i2c_fd file descriptor of the opened i2c module
 * @return temperature in decimal value
 */
double read_temperature_register(int i2c_fd) {

	double temp_read = 0;

	int8_t read_buffer[NBYTES_2];
	int8_t read_bytes = 0;

	if (write_pointer_register(i2c_fd, TEMPERATURE_REGISTER)) {
		ERROR_STDOUT("ERROR: while writing to the pointer register\n");
	}

	read_bytes = read(i2c_fd, &read_buffer, NBYTES_2);
	if (read_bytes < 0) {
		perror("ERROR: reading temp register");
	}

	temp_read = binary_to_decimal(read_buffer);

	return temp_read;
}

/**
 * @brief this function writes to the pointer register
 * @param i2c_fd file descriptor of the opened i2c module
 * @param reg register to be writtten/read
 * @return 0 if successful, 1 otherwise
 */
int8_t write_pointer_register(int i2c_fd, int8_t reg) {

	int ret = 0;
	ret = write(i2c_fd, &reg, sizeof(reg));

	if (ret < 0) {
		perror("write_pointer_register");
		return 1;
	}

	return 0;
}

/**
 * @brief this function initializes temperature sensor for I2C operations
 * @return void
 */
int init_temp_sensor() {

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

	return i2c_fd;

}

