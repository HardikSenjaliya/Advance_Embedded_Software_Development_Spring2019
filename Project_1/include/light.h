/*
 * light.h
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#ifndef SOURCE_LIGHT_H_
#define SOURCE_LIGHT_H_


#include <poll.h>

#include "utils.h"
#include "../include/APDS_9301_light_sensor.h"

void *run_light_sensor(void *params);

#endif /* SOURCE_LIGHT_H_ */
