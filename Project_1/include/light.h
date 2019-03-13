/*
 * light.h
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#ifndef SOURCE_LIGHT_H_
#define SOURCE_LIGHT_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "utils.h"

void *run_light_sensor(void *params);

#endif /* SOURCE_LIGHT_H_ */
