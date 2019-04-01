/*
 * light_sensor_test.c
 *
 *  Created on: Mar 30, 2019
 *      Author: hardyk
 */

#include "../include/APDS_9301_light_sensor.h"

#define LOWER_THRESHOLD_VALUE				(0x64)
#define UPPER_THRESHOLD_VALUE				(0x4E20)


void test_day_or_night(int i2c_fd);
void test_low_interrupt_threshold_register(int i2c_fd);
void test_high_interrupt_threshold_register(int i2c_fd);
void test_timing_register(int i2c_fd);

void test_day_or_night(int i2c_fd) {

	int8_t ret = day_or_night(150);
	if (ret == 0) {
		printf("test day or night - passed\n");
	} else {
		printf("test day or night - failed\n");
	}

	ret = day_or_night(50);
	if (ret == 1) {
		printf("test day or night - passed\n");
	} else {
		printf("test day or night - failed\n");
	}
}

void test_low_interrupt_threshold_register(int i2c_fd) {

	write_low_interrupt_threshold_register(i2c_fd, LOWER_THRESHOLD_VALUE);

	uint8_t ret = read_low_interrupt_threshold_register(i2c_fd);
	if(ret != LOWER_THRESHOLD_VALUE){
		printf("test low interrrupt threshold register - failed\n");
	}else{
		printf("test low interrupt threshold register - passed\n");
	}
}

void test_high_interrupt_threshold_register(int i2c_fd) {

	write_high_interrupt_threshold_register(i2c_fd, UPPER_THRESHOLD_VALUE);

	int16_t ret = read_high_interrupt_threshold_register(i2c_fd);

	if(ret == UPPER_THRESHOLD_VALUE){
		printf("high interrupt threshold register - passed\n");
	}else
	{
		printf("test high interrupt threshold register - failed\n");
	}
}

void test_timing_register(int i2c_fd) {


	write_timing_register(i2c_fd, 0x02);

	int ret = read_timing_register(i2c_fd);
	if(ret == 0x02){
		printf("test readning and writing timing register passed\n");
	}else{
		printf("test reading and writing timing register failed\n");
	}

}

void test_lux_conversion(uint16_t adc0_data, uint16_t adc1_data){

	double lux = convert_lumen_to_lux(adc0_data, adc1_data);

	if(lux > 0){
		printf("test lux conversion - passed\n");
	}else{
		printf("test lux conversion - failed\n");
	}
}


int main(void) {

	int i2c_fd = init_light_sensor();

	test_day_or_night(i2c_fd);
	test_low_interrupt_threshold_register(i2c_fd);
	test_high_interrupt_threshold_register(i2c_fd);
	test_timing_register(i2c_fd);
	test_lux_conversion(UPPER_THRESHOLD_VALUE, LOWER_THRESHOLD_VALUE);

}
