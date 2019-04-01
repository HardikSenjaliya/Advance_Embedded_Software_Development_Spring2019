/*
 * temp_sensor.c
 *	@brief this file is the i2c driver for temperature sensor
 *	and contains all the functions related to read and write of the
 *	temperature sensor.
 *
 *  Created on: Mar 30, 2019
 *      Author: hardyk
 */

#include "../include/TMP_102_temp_sensor.h"

#define PASS 		1
#define FAIL		0

#define TLOW_VALUE							(20)
#define THIGH_VALUE							(25)

void test_binary_to_decimal();
void test_convert_kelvin(int i2c_fd);
void test_convert_farenheit(int i2c_fd);
void test_tlow_register(int i2c_fd);
void test_thigh_reigster(int i2c_fd);
void test_config_register(int i2c_fd);

void test_binary_to_decimal() {

	uint8_t binary_data[2];
	binary_data[0] = 0x00;
	binary_data[1] = 0x32;

	double data = binary_to_decimal(binary_data);

	if (data == 50) {
		printf("Test Passed\n");
	} else {
		printf("Test Failed\n");
	}

	/*check for negative value*/
	binary_data[0] = 0x00;
	binary_data[1] = 0x87;

	int8_t n_data = binary_to_decimal(binary_data);
	if (n_data == -25) {
		printf("Test Passed\n");
	} else {
		printf("Test Failed\n");
	}

}

void test_convert_kelvin(int i2c_fd) {

	double temp = 0;
	double k_temp = convert_temp_kelvin(i2c_fd);

	if (k_temp != 273.5) {
		printf("test convert kelvin - failed\n");
	} else {
		printf("test convert kelvin - passed\n");
	}

}

void test_convert_farenheit(int i2c_fd) {

	double temp = 0;
	double k_far = convert_temp_farenheit(i2c_fd);

	if (k_far != -17.777) {
		printf("test convert farenheit - failed\n");
	} else {
		printf("test convert farenheit - passed\n");
	}

}

void test_tlow_register(int i2c_fd) {

	write_tlow_register(i2c_fd, TLOW_VALUE);

	uint16_t ret = read_tlow_register(i2c_fd);

	if (ret != TLOW_VALUE) {
		printf("test tlow register - failed\n");
	} else {
		printf("test tlow register - passed\n");
	}

}

void test_thigh_reigster(int i2c_fd) {

	write_thigh_register(i2c_fd, THIGH_VALUE);

	uint16_t ret = read_thigh_register(i2c_fd);

	if (ret != THIGH_VALUE) {
		printf("test thigh register - failed\n");
	} else {
		printf("test tlow register - passed\n");
	}

}

void test_config_register(int i2c_fd) {

	write_config_register(i2c_fd, 0xA060);

	uint16_t ret = read_config_register(i2c_fd);
	if (ret != 0x60A0) {
		printf("test config register - failed\n");
	} else {
		printf("test config register - passed\n");
	}

}

int main(void) {

	int result = 0;

	int i2c_fd = init_temp_sensor();

	test_binary_to_decimal(i2c_fd);
	test_config_register(i2c_fd);
	test_convert_farenheit(i2c_fd);
	test_convert_kelvin(i2c_fd);
	test_thigh_reigster(i2c_fd);
	test_tlow_register(i2c_fd);
}
