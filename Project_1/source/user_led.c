/*
 * user_led.c
 *	@brief this file is the library file for user leds
 *  Created on: Mar 28, 2019
 *      Author: hardyk
 */

#include "../include/user_led.h"

int fd_led_0 = 0, fd_led_1 = 0, fd_led_2 = 0, fd_led_3 = 0;

/**
 * @brief this function turns given user led on
 * @param led_no user led number
 */
void set_led_on(int led_no) {

	switch (led_no) {
	case 0:
		write(fd_led_0, LED_ON, 1);
		break;
	case 1:
		write(fd_led_1, LED_ON, 1);
		break;
	case 2:
		write(fd_led_2, LED_ON, 1);
		break;
	case 3:
		write(fd_led_3, LED_ON, 1);
		break;
	default:
		break;
	}
}

/**
 * @brief this function turns given user led off
 * @param led_no user led number
 */
void set_led_off(int led_no) {

	switch (led_no) {
	case 0:
		write(fd_led_0, LED_OFF, 1);
		break;
	case 1:
		write(fd_led_1, LED_OFF, 1);
		break;
	case 2:
		write(fd_led_2, LED_OFF, 1);
		break;
	case 3:
		write(fd_led_3, LED_OFF, 1);
		break;
	default:
		break;
	}
}

/**
 * @brief this function blinks given user led
 * @param led_no
 */
void blink_led(int led_no) {

	switch (led_no) {
	case 0:
		set_led_on(USER_LED0);
		usleep(1000);
		set_led_off(USER_LED0);
		break;
	case 1:
		set_led_on(USER_LED1);
		usleep(1000);
		set_led_off(USER_LED1);

		break;
	case 2:
		set_led_on(USER_LED2);
		usleep(1000);
		set_led_off(USER_LED2);
		break;
	case 3:
		set_led_on(USER_LED3);
		usleep(1000);
		set_led_off(USER_LED3);
		break;
	default:
		break;
	}
}

/**
 * @brief this function initialized user leds
 */
void init_leds(void) {

	fd_led_0 = open(USER_LED0_PIN, O_WRONLY);
	fd_led_1 = open(USER_LED1_PIN, O_WRONLY);
	fd_led_2 = open(USER_LED2_PIN, O_WRONLY);
	fd_led_3 = open(USER_LED3_PIN, O_WRONLY);

}
