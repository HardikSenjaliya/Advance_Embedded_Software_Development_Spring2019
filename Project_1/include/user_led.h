/*
 * user_led.h
 *
 *  Created on: Mar 28, 2019
 *      Author: hardyk
 */

#ifndef INCLUDE_USER_LED_H_
#define INCLUDE_USER_LED_H_

#include <linux/gpio.h>
#include <unistd.h>
#include <fcntl.h>

#define USER_LED0				(0)
#define USER_LED1				(1)
#define USER_LED2				(2)
#define USER_LED3				(3)

#define USER_LED0_PIN			"/sys/class/gpio/gpio53/value"
#define USER_LED1_PIN			"/sys/class/gpio/gpio54/value"
#define USER_LED2_PIN			"/sys/class/gpio/gpio55/value"
#define USER_LED3_PIN			"/sys/class/gpio/gpio56/value"

#define LED_ON					"1"
#define LED_OFF					"0"

void init_leds(void);
void set_led_off(int led_no);
void set_led_on(int led_no);
void blink_led(int led_no);

#endif /* INCLUDE_USER_LED_H_ */
