/*
 * temperature.h
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#ifndef SOURCE_TEMPERATURE_H_
#define SOURCE_TEMPERATURE_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "utils.h"


/*function prototypes*/
void *run_temperature_sensor(void *params);

#endif /* SOURCE_TEMPERATURE_H_ */
