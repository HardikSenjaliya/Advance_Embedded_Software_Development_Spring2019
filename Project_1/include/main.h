/*
 * main.h
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#ifndef SOURCE_MAIN_H_
#define SOURCE_MAIN_H_

#include "../include/utils.h"
#include "../include/light.h"
#include "../include/temperature.h"
#include "../include/logger.h"
#include "../include/socket.h"
#include "../include/APDS_9301_light_sensor.h"
#include "../include/TMP_102_temp_sensor.h"

int8_t startup_test(queue_descriptors_t *qDes);
int8_t logger_startup_test(queue_descriptors_t *qDes);
int8_t light_startup_test(queue_descriptors_t *qDes);
int8_t temp_startup_test(queue_descriptors_t *qDes);
int8_t socket_startup_test(queue_descriptors_t *qDes);

#endif /* SOURCE_MAIN_H_ */
