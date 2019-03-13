/*
 * main.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */


#include "../include/main.h"

/**
 * @brief main function
 * @return 0
 */

int main(void){

	pthread_t light_sensor, temperatture_sensor, logger;

	/*Spwan new threads*/

	/*1. light sensor thread*/
	if(pthread_create(&light_sensor, NULL, run_light_sensor, NULL))
		printf("ERROR: spawning light sensor thread\n");

	/*2. temperature sensor task*/
	if(pthread_create(&light_sensor, NULL, run_temperature_sensor, NULL))
		printf("ERROR: spawning temperature sensor thread\n");

	/*3. Logger task*/
	if(pthread_create(&light_sensor, NULL, run_logger, NULL))
		printf("ERROR: spawning logger thread\n");


	pthread_join(light_sensor, NULL);
	pthread_join(temperatture_sensor, NULL);
	pthread_join(logger, NULL);


	return 0;
}


